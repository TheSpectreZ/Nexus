#define NOMINMAX
#include "NxGraphics/TypeImpls.h"

#include "NxVulkan/VkShader.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkBuffer.h"
#include "NxVulkan/VkCommandQueue.h"
#include "NxVulkan/VkTexture.h"

#include "shaderc/shaderc.hpp"

#include "spirv_cross/spirv_glsl.hpp"
#include "spirv_cross/spirv_reflect.hpp"

static void GenerateReflections(const Nexus::SpirV* spirV, Nexus::ReflectionData* data, Nexus::ShaderStage stage)
{
	spirv_cross::CompilerGLSL reflector(*spirV);
	
	spirv_cross::ShaderResources resources = reflector.get_shader_resources();
	
	// Stage Input 
	for (const auto& resource : resources.stage_inputs)
	{
		//auto& type = reflector.get_type(resource.base_type_id);
		uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationLocation);
		
		NEXUS_LOG("Vulkan DEBUG", "%s Stage Input: %i",Nexus::GetShaderStageTypeStringName(stage).c_str(), binding);
	}

	// Uniform Buffers
	for (const auto& resource : resources.uniform_buffers)
	{
		auto activeBuffers = reflector.get_active_buffer_ranges(resource.id);

		if (activeBuffers.size())
		{
			
			auto& bufferType = reflector.get_type(resource.base_type_id);
			//int32_t memberCount = (int32_t)bufferType.member_types.size();
			uint32_t size = (uint32_t)reflector.get_declared_struct_size(bufferType);

			uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t set = reflector.get_decoration(resource.id, spv::DecorationDescriptorSet);

			auto& ref = data->bindings[set].emplace_back();
			ref.bindPoint = binding;
			ref.stage = stage;
			ref.type = Nexus::ShaderResourceType::Uniform;
			ref.bufferSize = size;
		}
	}

	// Sampled Images
	for (const auto& resource : resources.sampled_images)
	{
		uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationBinding);
		uint32_t set = reflector.get_decoration(resource.id, spv::DecorationDescriptorSet);

		//auto& baseType = reflector.get_type(resource.base_type_id);
		//auto& type = reflector.get_type(resource.type_id);

		//uint32_t dimension = baseType.image.dim;
		//uint32_t arraySize = type.array[0];

		auto& ref = data->bindings[set].emplace_back();
		ref.bindPoint = binding;
		ref.stage = stage;
		ref.type = Nexus::ShaderResourceType::SampledImage;
		ref.bufferSize = 0;
	}
}

Nexus::VulkanShader::VulkanShader(const ShaderSpecification& specs)
{
	{
		VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

		VkShaderModuleCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.codeSize = (uint32_t)specs.vertexData.size() * sizeof(uint32_t);
		Info.pCode = specs.vertexData.data();

		_VKR = vkCreateShaderModule(device, &Info, nullptr, &m_VertexModule);
		CHECK_HANDLE(m_VertexModule, VkShaderModule);

		Info.codeSize = (uint32_t)specs.fragmentData.size() * sizeof(uint32_t);
		Info.pCode = specs.fragmentData.data();

		_VKR = vkCreateShaderModule(device, &Info, nullptr, &m_FragmentModule);
		CHECK_HANDLE(m_FragmentModule, VkShaderModule);
	}

	NEXUS_LOG("Vulkan","Shader Created : %s", specs.filepath.c_str());

	GenerateReflections(&specs.vertexData, &m_ReflectionData, Nexus::ShaderStage::Vertex);
	GenerateReflections(&specs.fragmentData, &m_ReflectionData, Nexus::ShaderStage::Fragment);
	
	// Heap Layouts and Pool
	{
		NEXUS_LOG("Vulkan", "=================");
		NEXUS_LOG("Vulkan", "Shader Reflection\n");

		for (auto& [set, bindings] : m_ReflectionData.bindings)
		{
			for (auto& b : bindings)
			{
				NEXUS_LOG("Vulkan","Set: %i | BindPoint : %i, Type : %s, ShaderStage : %s",set, b.bindPoint, GetShaderResourceTypeStringName(b.type).c_str(), GetShaderStageTypeStringName(b.stage).c_str());
			}
			m_SetResource[set].HeapLayout = CreateRef<VulkanShaderResourceHeapLayout>(bindings);
			
			auto& pool = m_SetResource[set].HeapPools.emplace_back();
			pool = CreateRef<VulkanShaderResourcePool>(m_SetResource[set].HeapLayout, maxHeapCountPerPool);
			NEXUS_LOG("Vulkan", "");
		}
		NEXUS_LOG("Vulkan", "=================\n");
	}

	/////////////////////////////////
	// Pipeline layout
	/////////////////////////////////

	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		std::vector<VkDescriptorSetLayout> layouts;

		uint32_t size = (uint32_t)m_SetResource.size();
		for (uint32_t i = 0; i < size; i++)
		{
			layouts.push_back(m_SetResource[i].HeapLayout->Get());
		}

		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.setLayoutCount = (uint32_t)layouts.size();

		_VKR = vkCreatePipelineLayout(VulkanContext::Get()->GetDeviceRef()->Get(), &pipelineLayoutInfo, nullptr, &m_Layout);
		CHECK_HANDLE(m_Layout, VkPipelineLayout);
		NEXUS_LOG("Vulkan", "Shader Pipeline Created: Sets-%i", layouts.size());
	}

}

Nexus::VulkanShader::~VulkanShader()
{
	VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

	vkDestroyShaderModule(device, m_VertexModule, nullptr);
	vkDestroyShaderModule(device, m_FragmentModule, nullptr);

	vkDestroyPipelineLayout(device, m_Layout, nullptr);
	NEXUS_LOG("Vulkan", "Shader Pipeline Destroyed");

	m_SetResource.clear();
	
	NEXUS_LOG("Vulkan", "Shader Destroyed");
}

void Nexus::VulkanShader::AllocateShaderResourceHeap(ResourceHeapHandle handle)
{
	if (!m_SetResource[handle.set].GarbageHeaps.empty())
	{
		uint64_t id = 0;
		for (auto& i : m_SetResource[handle.set].GarbageHeaps)
		{
			id = i.first;
			break;
		}
		m_SetResource[handle.set].GarbageHeaps.erase(id);

		auto nh = m_SetResource[handle.set].Heaps.extract(id);
		nh.key() = handle.hashId;
		m_SetResource[handle.set].Heaps.insert(std::move(nh));	

		return;
	}

	VkDescriptorSet& Heap = m_SetResource[handle.set].Heaps[handle.hashId].Get();

	VkDescriptorSetAllocateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.descriptorSetCount = 1;
	Info.pSetLayouts = &m_SetResource[handle.set].HeapLayout->Get();

	bool allocSuccess = false;
	
	do
	{
		Info.descriptorPool = m_SetResource[handle.set].HeapPools.back()->Get();
		_VKR = vkAllocateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), &Info, &Heap);

		if (_VKR == VK_SUCCESS)
		{
			allocSuccess = true;
			NEXUS_LOG("Vulkan", "Shader Resource Heap Allocated: %i,%i", handle.set, handle.hashId);
		}
		else if (_VKR == VK_ERROR_OUT_OF_POOL_MEMORY)
		{
			auto& pool = m_SetResource[handle.set].HeapPools.emplace_back();
			pool = CreateRef<VulkanShaderResourcePool>(m_SetResource[handle.set].HeapLayout, maxHeapCountPerPool);
		}
		else
		{
			NEXUS_BREAK_ASSERT(1, "Vulkan Resource Heap allocation error");
		}

	} while (!allocSuccess);
}

void Nexus::VulkanShader::DeallocateShaderResourceHeap(ResourceHeapHandle handle)
{
	if (!m_SetResource.contains(handle.set))
		return;

	if (m_SetResource[handle.set].GarbageHeaps.contains(handle.hashId)) // Try Checking HashValue
		return;

	m_SetResource[handle.set].GarbageHeaps[handle.hashId] = true;
	NEXUS_LOG("Vulkan", "Shader Resource Heap Deallocated: %i,%i", handle.set, handle.hashId);
}

void Nexus::VulkanShader::BindShaderResourceHeap(ResourceHeapHandle handle)
{
	Ref<VulkanCommandQueue> cmd;// = DynamicPointerCast<VulkanCommandQueue>(Renderer::GetCommandQueue());

	VkDescriptorSet& Set = m_SetResource[handle.set].Heaps[handle.hashId].Get();
	vkCmdBindDescriptorSets(cmd->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, handle.set, 1, &Set, 0, nullptr);
}

void Nexus::VulkanShader::AllocateUniformBuffer(UniformBufferHandle handle)
{
	uint32_t size = 0;
	for (auto& b : m_ReflectionData.bindings[handle.set])
	{
		if (b.bindPoint == handle.binding)
		{
			size = b.bufferSize;
		}
	}

	NEXUS_BREAK_ASSERT((size == 0), "Uniform Buffer to be allocated has Size Zero");
	//ShaderLib::s_Instance->m_ResourcePool.AllocateUniformBuffer(handle.hashId, size);
}

void Nexus::VulkanShader::DeallocateUniformBuffer(UniformBufferHandle handle)
{
	//ShaderLib::s_Instance->m_ResourcePool.DeallocateUniformBuffer(handle.hashId);
}

void Nexus::VulkanShader::BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, UniformBufferHandle bufferhandle)
{
	Ref<VulkanUniformBuffer> Uniform;// = DynamicPointerCast<VulkanUniformBuffer>(ShaderLib::s_Instance->m_ResourcePool.GetUniformBuffer(bufferhandle.hashId));

	VkDescriptorBufferInfo Info{};
	Info.buffer = Uniform->Get();
	Info.range = Uniform->GetSize();
	Info.offset = 0;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.descriptorCount = 1;
	write.dstArrayElement = 0;
	write.dstSet = m_SetResource[heapHandle.set].Heaps[heapHandle.hashId].Get();
	write.dstBinding = bufferhandle.binding;
	write.pBufferInfo = &Info;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;
	
	//[Note] Optimize this to Do Together for every entity
	vkUpdateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), 1, &write, 0, nullptr);

	NEXUS_LOG("Vulkan", "Uniform Buffer Binded With Resource Heap: %i|%i : %i", heapHandle.set, bufferhandle.binding, Info.range);
}

void Nexus::VulkanShader::BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, CombinedImageSamplerHandle texHandle)
{
	Ref<VulkanTexture> texture = DynamicPointerCast<VulkanTexture>(texHandle.texture);
	Ref<VulkanSampler> sampler = DynamicPointerCast<VulkanSampler>(texHandle.sampler);

	VkDescriptorImageInfo Info{};
	Info.imageView = texture->Get();
	Info.sampler = sampler->Get();
	Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.descriptorCount = 1;
	write.dstArrayElement = 0;
	write.dstSet = m_SetResource[heapHandle.set].Heaps[heapHandle.hashId].Get();
	write.dstBinding = texHandle.binding;
	write.pImageInfo = &Info;
	write.pBufferInfo = nullptr;
	write.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), 1, &write, 0, nullptr);

	NEXUS_LOG("Vulkan", "Texture Binded With Resource Heap: %i|%i", heapHandle.set, texHandle.binding);
}

void Nexus::VulkanShader::SetUniformData(UniformBufferHandle handle, void* data)
{
	//ShaderLib::s_Instance->m_ResourcePool.GetUniformBuffer(handle.hashId)->Update(data);
}

VkShaderModule Nexus::VulkanShader::GetModule(VkShaderStageFlagBits flag)
{
	switch (flag)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return m_VertexModule;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return m_FragmentModule;
	default:
		NEXUS_ASSERT(1, "This Shader Type is Currently not Supported");
		return nullptr;
	}
}
