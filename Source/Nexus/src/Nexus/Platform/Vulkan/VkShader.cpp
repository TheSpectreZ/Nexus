#include "nxpch.h"
#include "VkShader.h"
#include "VkContext.h"

#include "shaderc/shaderc.hpp"

#include "spirv_cross/spirv_glsl.hpp"
#include "spirv_cross/spirv_reflect.hpp"

struct ReflectionData
{
	std::unordered_map<uint32_t, std::vector<Nexus::ShaderResouceHeapLayoutBinding>> bindings;
};

static void GenerateReflections(SpirV* spirV, ReflectionData* data, Nexus::ShaderStage stage)
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

	ReflectionData data;
	GenerateReflections(&vertexData, &data, Nexus::ShaderStage::Vertex);
	GenerateReflections(&fragmentData, &data, Nexus::ShaderStage::Fragment);

	// Heap Layouts and Pool
	{
		NEXUS_LOG_DEBUG("=================");
		NEXUS_LOG_INFO("Shader Reflection\n");

		for (auto& [set, bindings] : data.bindings)
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
}

void Nexus::VulkanShader::Destroy()
{
	VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

	vkDestroyShaderModule(device, m_VertexModule, nullptr);
	vkDestroyShaderModule(device, m_FragmentModule, nullptr);

	m_SetResource.clear();
	
	NEXUS_LOG_WARN("Vulkan Shader Destroyed");
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
