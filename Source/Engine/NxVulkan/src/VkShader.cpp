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

static VkShaderStageFlags GetVulkanShaderStage(Nexus::ShaderStage stage)
{
	switch (stage)
	{
	case Nexus::ShaderStage::Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case Nexus::ShaderStage::Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case Nexus::ShaderStage::Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	default:
		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}

static void GenerateReflections(const Nexus::SpirV* spirV, Nexus::ReflectionData* data, Nexus::ShaderStage stage)
{
	spirv_cross::CompilerGLSL reflector(*spirV);

	spirv_cross::ShaderResources resources = reflector.get_shader_resources();

	for (const auto& resource : resources.push_constant_buffers)
	{
		auto& bufferType = reflector.get_type(resource.base_type_id);
		uint32_t size = (uint32_t)reflector.get_declared_struct_size(bufferType);
		
		auto& ref = data->ranges[stage];
		ref.size = size;
		ref.stageFlags = GetVulkanShaderStage(stage);
		ref.offset = 0;
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

			auto& ref = data->bindings[set][binding];
			ref.bindPoint = binding;
			ref.stage = ref.stage | stage;
			ref.type = Nexus::ShaderResourceType::Uniform;
			ref.bufferSize = size;
			ref.arrayCount = 1;
		}
	}

	// Sampled Images
	for (const auto& resource : resources.sampled_images)
	{
		uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationBinding);
		uint32_t set = reflector.get_decoration(resource.id, spv::DecorationDescriptorSet);

		//auto& baseType = reflector.get_type(resource.base_type_id);

		//uint32_t dimension = baseType.image.dim;
		
		auto& type = reflector.get_type(resource.type_id);
		uint32_t arraySize = type.array[0];

		auto& ref = data->bindings[set][binding];
		ref.bindPoint = binding;
		ref.stage = ref.stage | stage;
		ref.type = Nexus::ShaderResourceType::SampledImage;
		ref.arrayCount = arraySize > 0 ? arraySize : 1;
		ref.bufferSize = 0;
	}
	
	// Storage Images
	for (const auto& resource : resources.storage_images)
	{
		uint32_t binding = reflector.get_decoration(resource.id, spv::DecorationBinding);
		uint32_t set = reflector.get_decoration(resource.id, spv::DecorationDescriptorSet);

		//auto& baseType = reflector.get_type(resource.base_type_id);
		auto& type = reflector.get_type(resource.type_id);
		uint32_t arraySize = type.array[0];

		//uint32_t dimension = baseType.image.dim;

		auto& ref = data->bindings[set][binding];
		ref.bindPoint = binding;
		ref.stage = ref.stage | stage;
		ref.type = Nexus::ShaderResourceType::StorageImage;
		ref.arrayCount = arraySize > 0 ? arraySize : 1;
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

		for (auto& [k, v] : specs.spirv)
		{
			Info.codeSize = (uint32_t)v.size() * sizeof(uint32_t);
			Info.pCode = v.data();

			_VKR = vkCreateShaderModule(device, &Info, nullptr, &m_Modules[k]);
			CHECK_HANDLE(m_Modules[k], VkShaderModule);

			GenerateReflections(&v, &m_ReflectionData, k);
		}
	}

	NEXUS_LOG("Vulkan","Shader Created : %s", specs.filepath.c_str());

	// Heap Layouts and Pool
	{
		NEXUS_LOG("Vulkan", "=================");
		NEXUS_LOG("Vulkan", "Shader Reflection\n");

		for (auto& [set, bindings] : m_ReflectionData.bindings)
		{
			std::vector< ShaderResouceHeapLayoutBinding> shBinds;
			for (uint32_t i = 0; i < bindings.size(); i++)
			{
				shBinds.emplace_back(bindings[i]);
				NEXUS_LOG("Vulkan","Set: %i | BindPoint : %i, Type : %s, ShaderStage : %s",set, bindings[i].bindPoint, GetShaderResourceTypeStringName(bindings[i].type).c_str(), GetShaderStageTypeStringName(bindings[i].stage).c_str());
			}
			m_SetResource[set].HeapLayout = CreateRef<VulkanShaderResourceHeapLayout>(shBinds);
			
			auto& pool = m_SetResource[set].HeapPools.emplace_back();
			pool = CreateRef<VulkanShaderResourcePool>(m_SetResource[set].HeapLayout, maxHeapCountPerPool);
			NEXUS_LOG("Vulkan", "");
		}

		for (auto& [k,r] : m_ReflectionData.ranges)
		{
			NEXUS_LOG("Vulkan", "PushConstant, Size : %i", r.size);
		}

		NEXUS_LOG("Vulkan", "");
		NEXUS_LOG("Vulkan", "=================\n");
	}

	/////////////////////////////////
	// Pipeline layout
	/////////////////////////////////

	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		std::vector<VkPushConstantRange> ranges{};
		for (auto& [k, r] : m_ReflectionData.ranges)
			ranges.push_back(r);

		pipelineLayoutInfo.pPushConstantRanges = ranges.data();
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)ranges.size();

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

	for(auto& [k,v] : m_Modules)
		vkDestroyShaderModule(device, v, nullptr);

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

	auto& sRes = m_SetResource[handle.set].Heaps[handle.hashId];
	VkDescriptorSet& Heap = sRes.Get();

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

void Nexus::VulkanShader::GetShaderResourceHeapLayoutBinding(ShaderResouceHeapLayoutBinding*& heap, uint32_t set,uint32_t binding)
{
	if (!m_ReflectionData.bindings.contains(set))
		return;

	if (binding < m_ReflectionData.bindings[set].size())
	{
		heap = &m_ReflectionData.bindings[set].at(binding);
	}
}

void Nexus::VulkanShader::BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, uint32_t binding, Ref<Buffer> buffer)
{
	Ref<VulkanBuffer> Uniform = DynamicPointerCast<VulkanBuffer>(buffer);

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
	write.dstBinding = binding;
	write.pBufferInfo = &Info;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;

	//[Note] Optimize this to Do Together for every entity
	vkUpdateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), 1, &write, 0, nullptr);

	NEXUS_LOG("Vulkan", "Uniform Buffer Binded With Resource Heap: %i|%i : %i", heapHandle.set, binding, Info.range);
}

void Nexus::VulkanShader::BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, ImageHandle texHandle)
{
	Ref<VulkanTexture> texture = DynamicPointerCast<VulkanTexture>(texHandle.texture);

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	
	VkDescriptorImageInfo Info{};
	Info.imageView = texture->Get(texHandle.miplevel);
	
	if (texHandle.Type == ShaderResourceType::SampledImage)
	{
		Ref<VulkanSampler> sampler = DynamicPointerCast<VulkanSampler>(texHandle.sampler);
		Info.sampler = sampler->Get();
		Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}
	else
	{
		Info.sampler = nullptr;
		Info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	}

	write.pImageInfo = &Info;
	write.descriptorCount = 1;
	write.dstArrayElement = texHandle.miplevel;
	write.dstSet = m_SetResource[heapHandle.set].Heaps[heapHandle.hashId].Get();
	write.dstBinding = texHandle.binding;
	write.pBufferInfo = nullptr;
	write.pTexelBufferView = nullptr;
	
	vkUpdateDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), 1, &write, 0, nullptr);

	NEXUS_LOG("Vulkan", "Texture Binded With Resource Heap: %i|%i", heapHandle.set, texHandle.binding);
}

VkShaderModule Nexus::VulkanShader::GetModule(VkShaderStageFlagBits flag)
{
	switch (flag)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return m_Modules[ShaderStage::Vertex];
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return m_Modules[ShaderStage::Fragment];
	case VK_SHADER_STAGE_COMPUTE_BIT:
		return m_Modules[ShaderStage::Compute];
	default:
		NEXUS_ASSERT(1, "This Shader Type is Currently not Supported");
		return nullptr;
	}
}
