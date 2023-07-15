#include "NxVulkan/VkPipeline.h"
#include "NxVulkan/VkShader.h"
#include "NxVulkan/VkShaderResource.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkRenderpass.h"

namespace Nexus
{
    static VkShaderStageFlags GetVulkanShaderStageFlag(ShaderStage Stage)
    {
        switch (Stage)
        {
        case Nexus::ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case Nexus::ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        default:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    }

    static VkPolygonMode GetVulkanPolygonMode(PolygonMode mode)
    {
        switch (mode)
        {
        case Nexus::PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;
        case Nexus::PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;
        default:
            return VK_POLYGON_MODE_MAX_ENUM;
        }
    }

    static VkCullModeFlags GetVulkanFaceCullMode(CullMode mode)
    {
        switch (mode)
        {
        case Nexus::CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case Nexus::CullMode::Back:
            return VK_CULL_MODE_FRONT_BIT;
        case Nexus::CullMode::None:
            return VK_CULL_MODE_NONE;
        default:
            return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        }
    }

    static VkFrontFace GetVulkanFrontFaceType(FrontFaceType Type)
    {
        switch (Type)
        {
        case Nexus::FrontFaceType::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        case Nexus::FrontFaceType::AntiClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default:
            return VK_FRONT_FACE_MAX_ENUM;
        }
    }

    static VkPrimitiveTopology GetVulkanTopology(TopologyType Type)
    {
        switch (Type)
        {
        case Nexus::TopologyType::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        default:
            return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        }
    }

}

Nexus::VulkanPipeline::VulkanPipeline(const PipelineSpecification& i)
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
    Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	VulkanSwapchain* swapchain = VulkanSwapchain::Get();

    /////////////////////////////////
    // Shader
    /////////////////////////////////

	Ref<VulkanShader> shader = DynamicPointerCast<VulkanShader>(i.shader);

    VkShaderModule vertShaderModule = shader->GetModule(VK_SHADER_STAGE_VERTEX_BIT);
    VkShaderModule fragShaderModule = shader->GetModule(VK_SHADER_STAGE_FRAGMENT_BIT);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    /////////////////////////////////
    // Vertex Input
    /////////////////////////////////
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;

    std::vector<VkVertexInputBindingDescription> bindings(i.vertexBindInfo.size());

    for (uint32_t k = 0; k < bindings.size(); k++)
    {
        bindings[k].binding = i.vertexBindInfo[k].binding;
        bindings[k].stride = i.vertexBindInfo[k].stride;

        if (i.vertexBindInfo[k].inputRate == VertexBindInfo::INPUT_RATE_VERTEX)
            bindings[k].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        if (i.vertexBindInfo[k].inputRate == VertexBindInfo::INPUT_RATE_INSTANCE)
            bindings[k].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    }

    
    std::vector<VkVertexInputAttributeDescription> attributes(i.vertexAttribInfo.size());
    
    for (uint32_t k = 0; k < attributes.size(); k++)
    {
        attributes[k].binding = i.vertexAttribInfo[k].binding;
        attributes[k].location = i.vertexAttribInfo[k].location;
        attributes[k].offset = i.vertexAttribInfo[k].offset;
        
        if(i.vertexAttribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC2)
            attributes[k].format = VK_FORMAT_R32G32_SFLOAT;
        if (i.vertexAttribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC3)
            attributes[k].format = VK_FORMAT_R32G32B32_SFLOAT;
        if (i.vertexAttribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC4)
            attributes[k].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    
    vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)bindings.size();
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributes.size();

    vertexInputInfo.pVertexBindingDescriptions = bindings.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

    /////////////////////////////////////////////////
    // Viewport (To-Do : Make This Data Dynamic)
    /////////////////////////////////////////////////

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    /////////////////////////////////
    // Input Assembly
    /////////////////////////////////

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    inputAssembly.topology = GetVulkanTopology(i.rasterizerInfo.topology);
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /////////////////////////////////
    // Rasterizer
    /////////////////////////////////

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = GetVulkanPolygonMode(i.rasterizerInfo.polygonMode);
    rasterizer.lineWidth = i.rasterizerInfo.lineWidth;
    rasterizer.cullMode = GetVulkanFaceCullMode(i.rasterizerInfo.cullMode);
    rasterizer.frontFace = GetVulkanFrontFaceType(i.rasterizerInfo.frontFace);
    rasterizer.depthBiasEnable = VK_FALSE;
    
    /////////////////////////////////
    // MultiSampling
    /////////////////////////////////

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = i.multisampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;

    /////////////////////////////////
    // Blending
    /////////////////////////////////

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    /////////////////////////////////
    // Depth Stencil
    /////////////////////////////////

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};

    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = {};
    depthStencilState.back = {};

    /////////////////////////////////
    // Dynamic States
    /////////////////////////////////

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    /////////////////////////////////
    // Graphics Pipeline
    /////////////////////////////////

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.layout = shader->GetPipelineLayout();
    pipelineInfo.renderPass = DynamicPointerCast<VulkanRenderpass>(i.renderpass)->Get();
    pipelineInfo.subpass = i.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    _VKR = vkCreateGraphicsPipelines(device->Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
    CHECK_HANDLE(m_Pipeline, VkPipeline);
    NEXUS_LOG("Vulkan", "Pipeline Created");
}

Nexus::VulkanPipeline::~VulkanPipeline()
{
    VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();

    vkDestroyPipeline(device, m_Pipeline, nullptr);

    NEXUS_LOG("Vulkan", "Pipeline Destroyed");
}
