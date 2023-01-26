#include "Graphics/Pipeline.h"
#include "Backend.h"
#include "vkAssert.h"

#include "shaderc/shaderc.hpp"
#include <fstream>
#include <sstream>

void Nexus::Graphics::PipelineLayout::Create(std::vector<VkDescriptorSetLayout> layout, std::vector<VkPushConstantRange> ranges)
{
	VkPipelineLayoutCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = 0;

	Info.setLayoutCount = (uint32_t)layout.size();
	Info.pSetLayouts = layout.data();

	Info.pushConstantRangeCount = (uint32_t)ranges.size();
	Info.pPushConstantRanges = ranges.data();

	vkCreatePipelineLayout(Backend::GetDevice(), &Info, nullptr, &m_handle);
}

void Nexus::Graphics::PipelineLayout::Destroy()
{
	vkDestroyPipelineLayout(Backend::GetDevice(), m_handle, nullptr);
}

static shaderc_shader_kind GetShaderCType(VkShaderStageFlagBits type)
{
	switch (type)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return shaderc_glsl_vertex_shader;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return shaderc_glsl_fragment_shader;
	default:
		return (shaderc_shader_kind)0;
	}
}

void Nexus::Graphics::GraphicsPipeline::Create(const PipelineCreateInfo& Info)
{
	VkPipelineDynamicStateCreateInfo DynamicInfo{};
	DynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicInfo.dynamicStateCount = (uint32_t)(Info.dynamicStates.size());
	DynamicInfo.pDynamicStates = Info.dynamicStates.data();

	VkPipelineVertexInputStateCreateInfo VertexInfo{};
	VertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInfo.pNext = nullptr;
	VertexInfo.flags = 0;
	VertexInfo.vertexAttributeDescriptionCount = (uint32_t)(Info.vertexAttributes.size());
	VertexInfo.pVertexAttributeDescriptions = Info.vertexAttributes.data();
	VertexInfo.vertexBindingDescriptionCount = (uint32_t)(Info.vertexBindings.size());
	VertexInfo.pVertexBindingDescriptions = Info.vertexBindings.data();

	VkPipelineInputAssemblyStateCreateInfo InputState{};
	InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputState.flags = 0;
	InputState.pNext = nullptr;
	InputState.topology = Info.topology;
	InputState.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo ViewportState{};
	ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportState.pNext = nullptr;
	ViewportState.flags = 0;
	ViewportState.scissorCount = Info.scissorCount;
	ViewportState.pScissors = Info.pScissors;
	ViewportState.viewportCount = Info.viewportCount;
	ViewportState.pViewports = Info.pViewports;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.rasterizationSamples = Info.samples;
	multisampling.minSampleShading = 0.2f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo RasterInfo{};
	RasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterInfo.depthClampEnable = VK_FALSE;
	RasterInfo.rasterizerDiscardEnable = VK_FALSE;
	RasterInfo.polygonMode = Info.polygonMode;
	RasterInfo.lineWidth = 1.f;
	RasterInfo.cullMode = Info.cullMode;
	RasterInfo.frontFace = Info.frontFace;
	RasterInfo.depthBiasClamp = VK_FALSE;

	// Shaders
	std::vector<VkShaderModule> shaders;
	for (auto& p : Info.ShaderPaths)
	{
		std::string Shader;
		std::ifstream is(p.first, std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			uint32_t shaderSize = (uint32_t)is.tellg();
			is.seekg(0, std::ios::beg);

			Shader.resize(shaderSize);
			is.read(&Shader[0], shaderSize);
			is.close();

		}
		else
		{
			std::stringstream ss;
			ss << "Can't Open ShaderPath :" << p.first;
			NEXUS_ASSERT(1,ss.str().c_str());
		}
		
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult result =
			compiler.CompileGlslToSpv(Shader, GetShaderCType(p.second), p.first.c_str(), options);

		NEXUS_ASSERT((result.GetCompilationStatus() != shaderc_compilation_status_success),result.GetErrorMessage().c_str());

		std::vector<uint32_t> SpirV = std::vector<uint32_t>(result.cbegin(), result.cend());

		VkShaderModuleCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.codeSize = static_cast<uint32_t>(SpirV.size()) * static_cast<size_t>(sizeof(4));
		Info.pCode = SpirV.data();

		vkCreateShaderModule(Backend::GetDevice(), &Info, nullptr, &shaders.emplace_back());

		NEXUS_LOG_INFO("Shader Module Created: {0}", p.first);
	}
	
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(shaders.size());

	for (uint32_t i = 0; i < (uint32_t)shaderStages.size(); i++)
	{
		shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[i].pNext = nullptr;
		shaderStages[i].pName = "main";
		shaderStages[i].module = shaders[i];
		shaderStages[i].stage = Info.ShaderPaths[i].second;
		shaderStages[i].pSpecializationInfo = nullptr;
	}

	VkGraphicsPipelineCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	info.basePipelineHandle = Info.basePipeline;
	info.basePipelineIndex = Info.basePipelineIndex;
	info.renderPass = Info.renderPass;
	info.subpass = Info.subPassIndex;
	info.pDepthStencilState = &Info.depthStencil;
	info.pColorBlendState = &Info.colorBlend;
	info.pStages = shaderStages.data();
	info.stageCount = (uint32_t)shaderStages.size();
	
	info.pDynamicState = &DynamicInfo;
	info.pVertexInputState = &VertexInfo;
	info.pInputAssemblyState = &InputState;
	info.pViewportState = &ViewportState;
	info.pMultisampleState = &multisampling;
	info.pRasterizationState = &RasterInfo;

	_VKR = vkCreateGraphicsPipelines(Backend::GetDevice(), VK_NULL_HANDLE, 1, &info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkPipeline)
	NEXUS_LOG_WARN("Pipeline Created")

	for (auto& s : shaders)
		vkDestroyShaderModule(Backend::GetDevice(), s, nullptr);
}

void Nexus::Graphics::GraphicsPipeline::Destroy()
{
	vkDestroyPipeline(Backend::GetDevice(), m_handle, nullptr);
}

