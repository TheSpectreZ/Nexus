#include "nxpch.h"
#include "VkShader.h"
#include "VkContext.h"

#include "shaderc/shaderc.hpp"

#include "spirv_cross/spirv_glsl.hpp"
 
/*
 
	TODO:

	- Generate Reflection before Compiling
	- Store Reflection data to Create Descriptor Set Layout
	- Use This Descriptor Set Layout with Some Other Data to Create a Pipeline Object

	- Shader Shouldn't be the focal point of this thing, Make it so that ShaderFile has Pipeline
		Data ; it is essentially a pipeline description file
	- Then make Pipeline with Pipeline layout generated from Shader

*/

static void GenerateReflections(SpirV& spirV)
{
	spirv_cross::Compiler compiler(std::move(spirV));
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	// Uniform Buffers
	for (const auto& resource : resources.uniform_buffers)
	{
		auto activeBuffers = compiler.get_active_buffer_ranges(resource.id);

		if (activeBuffers.size())
		{
			auto& name = resource.name;
			
			auto& bufferType = compiler.get_type(resource.base_type_id);
			int32_t memberCount = (int32_t)bufferType.member_types.size();
			uint32_t size = (uint32_t)compiler.get_declared_struct_size(bufferType);

			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			NEXUS_LOG_DEBUG("Uniform Buffer: {0} | binding: {1} | set: {2} | size: {3} | memberCount: {4}",name,binding,set,size,memberCount);
		}
	}

	// Sampled Images
	for (const auto& resource : resources.sampled_images)
	{
		auto& name = resource.name;
		auto& baseType = compiler.get_type(resource.base_type_id);
		auto& type = compiler.get_type(resource.type_id);

		uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

		uint32_t dimension = baseType.image.dim;
		uint32_t arraySize = type.array[0];

		NEXUS_LOG_DEBUG("Sampled Image: {0},{1},{2},{3},{4}", name, binding, set, dimension, arraySize);
	}
}

Nexus::VulkanShader::VulkanShader(SpirV& vertexData, SpirV& fragmentData,const char* Filepath)
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

	// Generating Reflections
	//{
	//	GenerateReflections(vertexData);
	//	GenerateReflections(fragmentData);
	//}

}

Nexus::VulkanShader::~VulkanShader()
{
	VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

	vkDestroyShaderModule(device, m_VertexModule, nullptr);
	vkDestroyShaderModule(device, m_FragmentModule, nullptr);

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
