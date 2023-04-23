#include "nxpch.h"
#include "VkShader.h"
#include "VkContext.h"
#include "VkBuffer.h"
#include "VkCommand.h"

#include "shaderc/shaderc.hpp"

#include "spirv_cross/spirv_glsl.hpp"
#include "spirv_cross/spirv_reflect.hpp"

static void GenerateReflections(SpirV* spirV, Nexus::ReflectionData* data, Nexus::ShaderStage stage)
{
	spirv_cross::CompilerGLSL reflector(std::move(*spirV));
	
	spirv_cross::ShaderResources resources = reflector.get_shader_resources();
	
	// Push Constants
	for (const auto& resource : resources.uniform_buffers)
	{
		auto& name = resource.name;
		
		auto& bufferType = reflector.get_type(resource.base_type_id);
		int32_t memberCount = (int32_t)bufferType.member_types.size();
		uint32_t size = (uint32_t)reflector.get_declared_struct_size(bufferType);
	}

	// Uniform Buffers
	for (const auto& resource : resources.uniform_buffers)
	{
		auto activeBuffers = reflector.get_active_buffer_ranges(resource.id);

		if (activeBuffers.size())
		{
			auto& name = resource.name;
			
			auto& bufferType = reflector.get_type(resource.base_type_id);
			int32_t memberCount = (int32_t)bufferType.member_types.size();
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
		auto& name = resource.name;
		auto& baseType = reflector.get_type(resource.base_type_id);
		auto& type = reflector.get_type(resource.type_id);

		uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationBinding);
		uint32_t set = reflector.get_decoration(resource.id, spv::DecorationDescriptorSet);

		uint32_t dimension = baseType.image.dim;
		uint32_t arraySize = type.array[0];

		auto& ref = data->bindings[set].emplace_back();
		ref.bindPoint = binding;
		ref.stage = stage;
		ref.type = Nexus::ShaderResourceType::SampledImage;
		ref.bufferSize = 0;
	}
}

Nexus::VulkanShader::VulkanShader(SpirV& vertexData, SpirV& fragmentData, const char* Filepath)
{
	// Compiling Source
	{
		VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

		VkShaderModuleCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.codeSize = (uint32_t)vertexData.size() * sizeof(uint32_t);
		Info.pCode = vertexData.data();

		_VKR = vkCreateShaderModule(device, &Info, nullptr, &m_VertexModule);
		CHECK_HANDLE(m_VertexModule, VkShaderModule);

		Info.codeSize = (uint32_t)fragmentData.size() * sizeof(uint32_t);
		Info.pCode = fragmentData.data();

		_VKR = vkCreateShaderModule(device, &Info, nullptr, &m_FragmentModule);
		CHECK_HANDLE(m_FragmentModule, VkShaderModule);
	}

	NEXUS_LOG_WARN("Vulkan Shader Created: {0}", Filepath);

	GenerateReflections(&vertexData, &m_ReflectionData, Nexus::ShaderStage::Vertex);
	GenerateReflections(&fragmentData, &m_ReflectionData, Nexus::ShaderStage::Fragment);

	
	// Heap Layouts and Pool
	{
		NEXUS_LOG_DEBUG("=================");
		NEXUS_LOG_INFO("Shader Reflection\n");

		for (auto& [set, bindings] : m_ReflectionData.bindings)
		{
			for (auto& b : bindings)
			{
				NEXUS_LOG_TRACE("Set: {0} | BindPoint: {1}, Type: {2}, ShaderStage: {3}",set, b.bindPoint, GetShaderResourceTypeStringName(b.type).c_str(), GetShaderStageTypeStringName(b.stage).c_str());
			}
			m_SetResource[set].HeapLayout = CreateRef<VulkanShaderResourceHeapLayout>(bindings);
			
			auto& pool = m_SetResource[set].HeapPools.emplace_back();
			pool = CreateRef<VulkanShaderResourcePool>(m_SetResource[set].HeapLayout, maxHeapCountPerPool);
			NEXUS_LOG_DEBUG("");
		}
		NEXUS_LOG_DEBUG("=================\n");
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

		for (auto& [k, v] : m_SetResource)
		{
			layouts.push_back(v.HeapLayout->Get());
		}

		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.setLayoutCount = (uint32_t)layouts.size();

		_VKR = vkCreatePipelineLayout(VulkanContext::Get()->GetDeviceRef()->Get(), &pipelineLayoutInfo, nullptr, &m_Layout);
		CHECK_HANDLE(m_Layout, VkPipelineLayout);
		NEXUS_LOG_TRACE("Vulkan Shader Pipeline Created: Sets-{0}", layouts.size());
	}

}

void Nexus::VulkanShader::Destroy()
{
	VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

	vkDestroyShaderModule(device, m_VertexModule, nullptr);
	vkDestroyShaderModule(device, m_FragmentModule, nullptr);

	vkDestroyPipelineLayout(device, m_Layout, nullptr);

	m_SetResource.clear();
	
	NEXUS_LOG_WARN("Vulkan Shader Destroyed");
}

void Nexus::VulkanShader::AllocateShaderResourceHeap(uint64_t hashId, uint32_t set)
{
	if (!m_SetResource[set].GarbageHeaps.empty())
	{
		uint64_t id = 0;
		for (auto& i : m_SetResource[set].GarbageHeaps)
		{
			id = i.first;
			break;
		}
		m_SetResource[set].GarbageHeaps.erase(id);

		auto nh = m_SetResource[set].Heaps.extract(id);
		nh.key() = hashId;
		m_SetResource[set].Heaps.insert(std::move(nh));	

		return;
	}

	VkDescriptorSet& Heap = m_SetResource[set].Heaps[hashId].Get();

	VkDescriptorSetAllocateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.descriptorSetCount = 1;
	Info.pSetLayouts = &m_SetResource[set].HeapLayout->Get();

	bool allocSuccess = false;
	
	do
	{
		Info.descriptorPool = m_SetResource[set].HeapPools.back()->Get();
		_VKR = vkAllocateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), &Info, &Heap);

		if (_VKR == VK_SUCCESS)
		{
			allocSuccess = true;
			NEXUS_LOG_TRACE("Vulkan Shader Resource Heap Allocated: {0},{1}", set, hashId);
		}
		else if (_VKR == VK_ERROR_OUT_OF_POOL_MEMORY)
		{
			auto& pool = m_SetResource[set].HeapPools.emplace_back();
			pool = CreateRef<VulkanShaderResourcePool>(m_SetResource[set].HeapLayout, maxHeapCountPerPool);
		}
		else
		{
			NEXUS_ASSERT(1, "Vulkan Resource Heap allocation error");
		}

	} while (!allocSuccess);
}

void Nexus::VulkanShader::DeallocateShaderResourceHeap(uint64_t hashId, uint32_t set)
{
	if (!m_SetResource.contains(set))
		return;

	if (m_SetResource[set].GarbageHeaps.contains(hashId)) // Try Checking HashValue
		return;

	m_SetResource[set].GarbageHeaps[hashId] = true;
	NEXUS_LOG_TRACE("Vulkan Shader Resource Heap Deallocated: {0},{1}", set, hashId);
}

void Nexus::VulkanShader::BindShaderResourceHeap(uint64_t hashId, uint32_t set)
{
	Ref<VulkanCommand> cmd = DynamicPointerCast<VulkanCommand>(Command::GetRef());

	VkDescriptorSet& Set = m_SetResource[set].Heaps[hashId].Get();
	vkCmdBindDescriptorSets(cmd->m_RenderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Layout, set, 1, &Set, 0, nullptr);
}

void Nexus::VulkanShader::AllocateUniformBuffer(uint64_t hashId, uint32_t set, uint32_t binding)
{
	uint32_t size = 0;
	for (auto& b : m_ReflectionData.bindings[set])
	{
		if (b.bindPoint == binding)
		{
			size = b.bufferSize;
		}
	}

	NEXUS_ASSERT((size == 0), "Uniform Buffer to be allocated has Size Zero");
	ShaderLib::s_Instance->m_ResourcePool.AllocateUniformBuffer(hashId, size);
}

void Nexus::VulkanShader::DeallocateUniformBuffer(uint64_t hashId)
{
	ShaderLib::s_Instance->m_ResourcePool.DeallocateUniformBuffer(hashId);
}

void Nexus::VulkanShader::BindUniformWithResourceHeap(uint64_t uniformId, uint64_t heapId, uint32_t set, uint32_t binding)
{
	Ref<VulkanUniformBuffer> Uniform = DynamicPointerCast<VulkanUniformBuffer>(ShaderLib::s_Instance->m_ResourcePool.GetUniformBuffer(uniformId));

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
	write.dstSet = m_SetResource[set].Heaps[heapId].Get();
	write.pBufferInfo = &Info;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;
	
	// Optimize this to Do Together for every entity
	vkUpdateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), 1, &write, 0, nullptr);

	NEXUS_LOG_TRACE("Vulkan Uniform Buffer Binded With Resource Heap: {0}|{1} : {2}", set, binding, Info.range);
}

void Nexus::VulkanShader::SetUniformData(uint64_t uniformId, void* data)
{
	ShaderLib::s_Instance->m_ResourcePool.GetUniformBuffer(uniformId)->Update(data);
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