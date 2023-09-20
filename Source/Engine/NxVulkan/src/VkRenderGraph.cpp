#include "NxVulkan/VkRenderGraph.h"
#include "NxVulkan/VkShader.h"

static VkFormat GetVulkanTextureFormat(Nexus::TextureFormat format)
{
	switch (format)
	{
	case Nexus::TextureFormat::SWAPCHAIN_COLOR:
		return Nexus::VulkanSwapchain::Get()->GetImageFormat();
	case Nexus::TextureFormat::SWAPCHAIN_DEPTH:
		return Nexus::VulkanContext::Get()->GetPhysicalDeviceRef()->GetDepthFormat();
	case Nexus::TextureFormat::RGBA8_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;
	case Nexus::TextureFormat::RG16_SFLOAT:
		return VK_FORMAT_R16G16_SFLOAT;
	case Nexus::TextureFormat::RG32_SFLOAT:
		return VK_FORMAT_R32G32_SFLOAT;
	case Nexus::TextureFormat::RGBA16_SFLOAT:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case Nexus::TextureFormat::RGBA32_SFLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

static VkImageUsageFlags GetVulkanRenderTargetUsage(Nexus::RenderTargetUsage usage)
{
	switch (usage)
	{
	case Nexus::RenderTargetUsage::ColorAttachment:
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	case Nexus::RenderTargetUsage::DepthAttachment:
		return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	default:
		return VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
	}
}

static VkImageAspectFlags GetVulkanImageAspectFlag(Nexus::RenderTargetUsage usage)
{
	switch (usage)
	{
	case Nexus::RenderTargetUsage::ColorAttachment:
		return VK_IMAGE_ASPECT_COLOR_BIT;
	case Nexus::RenderTargetUsage::DepthAttachment:
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	case Nexus::RenderTargetUsage::BackBuffer:
		return VK_IMAGE_ASPECT_COLOR_BIT;
	default:
		return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
	}
}

static VkImageLayout GetVulkanImageLayoutFromUsage(Nexus::RenderTargetUsage usage)
{
	switch (usage)
	{
	case Nexus::RenderTargetUsage::ColorAttachment:
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case Nexus::RenderTargetUsage::DepthAttachment:
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	case Nexus::RenderTargetUsage::BackBuffer:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default:
		return VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}

static VkPrimitiveTopology GetVulkanPipelineTopology(Nexus::RenderPipelineTopology topology)
{
	switch (topology)
	{
	case Nexus::RenderPipelineTopology::TriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case Nexus::RenderPipelineTopology::LineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	default:
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}
}

static VkPolygonMode GetVulkanPipelinePolygonMode(Nexus::RenderPipelinePolygonMode polygon)
{
	switch (polygon)
	{
	case Nexus::RenderPipelinePolygonMode::Fill:
		return VK_POLYGON_MODE_FILL;
	case Nexus::RenderPipelinePolygonMode::Line:
		return VK_POLYGON_MODE_LINE;
	default:
		return VK_POLYGON_MODE_MAX_ENUM;
	}
}

static VkCullModeFlags GetVulkanPipelineCullMode(Nexus::RenderPipelineCullMode cullMode)
{
	switch (cullMode)
	{
	case Nexus::RenderPipelineCullMode::Front:
		return VK_CULL_MODE_FRONT_BIT;
	case Nexus::RenderPipelineCullMode::Back:
		return VK_CULL_MODE_BACK_BIT;
	case Nexus::RenderPipelineCullMode::Front_Back:
		return VK_CULL_MODE_FRONT_AND_BACK;
	case Nexus::RenderPipelineCullMode::None:
		return VK_CULL_MODE_NONE;
	default:
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

static VkFrontFace GetVulkanPipelineFrontFaceType(Nexus::RenderPiplineFrontFaceType frontFace)
{
	switch (frontFace)
	{
	case Nexus::RenderPiplineFrontFaceType::Clockwise:
		return VK_FRONT_FACE_CLOCKWISE;
	case Nexus::RenderPiplineFrontFaceType::AntiClockwise:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		return VK_FRONT_FACE_MAX_ENUM;
	}
}

Nexus::VulkanRenderGraph::VulkanRenderGraph()
{
	auto context = VulkanContext::Get();
	m_Device = context->GetDeviceRef();
	m_pDevice = context->GetPhysicalDeviceRef();
}

Nexus::VulkanRenderGraph::~VulkanRenderGraph()
{
	for (auto& [k, v] : m_Passes)
		v.Destroy(m_Device->Get());
	
	for (auto& [k, v] : m_Attachments)
		v.Destroy(m_Device->Get(), m_Device->GetAllocator());
}

void Nexus::VulkanRenderGraph::Bake()
{
	for (auto& [name, specs] : m_GPUpasses)
	{
		for (auto& output : specs.getOutputs())
			m_Attachments[output].Create(m_RenderTargets[output], specs.getRenderTargetExtent());

		m_Passes[name].Create(specs, this);

		for (auto& pipeline : specs.getGraphicsPipelines())
			m_GraphicsPipelines[pipeline].Create(m_GraphicsRenderPipelines[pipeline], m_Passes[name]);
	}

	NEXUS_LOG("Vulkan", "RenderGraph Bake Complete");
}

void Nexus::VulkanRenderTarget::Create(const RenderTargetSpecification& specs,Extent rtExtent)
{
	uint32_t count = VulkanSwapchain::Get()->GetImageCount();
	
	auto device = VulkanContext::Get()->GetDeviceRef();
	auto gpu = VulkanContext::Get()->GetPhysicalDeviceRef();

	m_Handles.resize(count);
	for (uint32_t i = 0; i < count; i++)
	{
		if (specs.usage == RenderTargetUsage::BackBuffer)
		{
			m_Handles[i].view = VulkanSwapchain::Get()->GetImageView(i);
			continue;
		}

		// Image
		{
			auto sExtent = VulkanSwapchain::Get()->GetExtent();

			VkExtent3D extent{ 0,0,1 };
			extent.width = rtExtent.width > 0 ? rtExtent.width : sExtent.width;
			extent.height = rtExtent.height > 0 ? rtExtent.height : sExtent.height;

			VkImageCreateInfo Info{};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			Info.pNext = nullptr;
			Info.extent = extent;
			Info.arrayLayers = 1;
			Info.mipLevels = 1;
			Info.flags = 0;
			Info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			Info.pQueueFamilyIndices = nullptr;
			Info.queueFamilyIndexCount = 0;
			Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			Info.tiling = VK_IMAGE_TILING_OPTIMAL;
			Info.imageType = VK_IMAGE_TYPE_2D;
			Info.samples = specs.multiSampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;
			Info.format = GetVulkanTextureFormat(specs.format);
			Info.usage = GetVulkanRenderTargetUsage(specs.usage);

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

			vmaCreateImage(device->GetAllocator(), &Info, &allocInfo, &m_Handles[i].image, &m_Handles[i].alloc, nullptr);
		}

		// View
		{
			VkImageViewCreateInfo Info{};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			Info.pNext = nullptr;
			Info.flags = 0;
			Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			Info.image = m_Handles[i].image;
			Info.components.r = VK_COMPONENT_SWIZZLE_R;
			Info.components.g = VK_COMPONENT_SWIZZLE_G;
			Info.components.b = VK_COMPONENT_SWIZZLE_B;
			Info.components.a = VK_COMPONENT_SWIZZLE_A;
			Info.format = GetVulkanTextureFormat(specs.format);
			Info.subresourceRange.aspectMask = GetVulkanImageAspectFlag(specs.usage);
			Info.subresourceRange.layerCount = 1;
			Info.subresourceRange.levelCount = 1;
			Info.subresourceRange.baseArrayLayer = 0;
			Info.subresourceRange.baseMipLevel = 0;

			vkCreateImageView(device->Get(), &Info, nullptr, &m_Handles[i].view);
		}
	}
}

void Nexus::VulkanRenderTarget::Destroy(VkDevice device, VmaAllocator allocator)
{
	for (auto& handle : m_Handles)
	{
		if (!handle.image)
			continue;

		vkDestroyImageView(device, handle.view, nullptr);
		vmaDestroyImage(allocator, handle.image, handle.alloc);
	}
}

void Nexus::VulkanRenderGraphPass::Create(const RenderGraphPassSpecification& specs, VulkanRenderGraph* graph)
{
	auto device = VulkanContext::Get()->GetDeviceRef();
	auto gpu = VulkanContext::Get()->GetPhysicalDeviceRef();

	std::vector<VkAttachmentReference> colorRefs;

	bool hasDepthRef = false;
	VkAttachmentReference depthRef{};

	std::vector<VkAttachmentDescription> attachments;
	for (auto& outputName : specs.getOutputs())
	{
		auto& rSpecs = graph->m_RenderTargets[outputName];

		auto& desc = attachments.emplace_back();
		desc.format = GetVulkanTextureFormat(rSpecs.format);
		desc.samples = rSpecs.multiSampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;
		desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout = GetVulkanImageLayoutFromUsage(rSpecs.usage);
		desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.flags = 0;

		// [To-Do]: This will depend on the Entire Render-Graph: Configure this !!!!
		desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		if (rSpecs.usage == RenderTargetUsage::DepthAttachment)
		{
			hasDepthRef = true;

			depthRef.layout = desc.finalLayout;
			depthRef.attachment = (uint32_t)attachments.size() - 1;
		}
		else
		{
			auto& ref = colorRefs.emplace_back();
			ref.layout = desc.finalLayout;
			ref.attachment = (uint32_t)attachments.size() - 1;
		}
	}

	m_colorAttachmentCount = (uint32_t)colorRefs.size();

	VkSubpassDescription subpass{};
	subpass.flags = 0;
	subpass.colorAttachmentCount = (uint32_t)colorRefs.size();
	subpass.pColorAttachments = colorRefs.data();
	subpass.pDepthStencilAttachment = hasDepthRef ? &depthRef : nullptr;
	subpass.pResolveAttachments = nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	Info.flags = 0;
	Info.pAttachments = attachments.data();
	Info.attachmentCount = (uint32_t)attachments.size();
	Info.pSubpasses = &subpass;
	Info.subpassCount = 1;
	Info.pDependencies = nullptr;
	Info.dependencyCount = 0;

	_VKR = vkCreateRenderPass(device->Get(), &Info, nullptr, &m_Handle);
	CHECK_LOG_VKR;

	m_Framebuffer.Create(specs, m_Handle, graph);
}

void Nexus::VulkanRenderGraphPass::Destroy(VkDevice device)
{
	m_Framebuffer.Destroy(device);
	vkDestroyRenderPass(device, m_Handle, nullptr);
}

void Nexus::VulkanRenderGraphFrameBuffer::Create(const RenderGraphPassSpecification& specs, VkRenderPass& pass, VulkanRenderGraph* graph)
{
	auto extent = VulkanSwapchain::Get()->GetExtent();

	uint32_t count = VulkanSwapchain::Get()->GetImageCount();
	m_Handles.resize(count);
	
	for (uint32_t i = 0; i < count; i++)
	{
		auto& outputs = specs.getOutputs();

		std::vector<VkImageView> views;

		for (auto& name : outputs)
		{
			auto& renderTarget = graph->m_Attachments[name];
			views.push_back(renderTarget.Get(i));
		}

		VkFramebufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.layers = 1;
		Info.renderPass = pass;
		Info.pAttachments = views.data();
		Info.attachmentCount = (uint32_t)views.size();
		Info.width = extent.width;
		Info.height = extent.height;

		_VKR = vkCreateFramebuffer(graph->m_Device->Get(), &Info, nullptr, &m_Handles[i]);
		CHECK_LOG_VKR;
	}
}

void Nexus::VulkanRenderGraphFrameBuffer::Destroy(VkDevice device)
{
	for (auto& fb : m_Handles)
		vkDestroyFramebuffer(device, fb, nullptr);
}

void Nexus::VulkanGraphicsRenderPipeline::Create(const GraphicsRenderPipelineSpecification& specs, VulkanRenderGraphPass& renderPass)
{
	auto device = VulkanContext::Get()->GetDeviceRef();
	auto gpu = VulkanContext::Get()->GetPhysicalDeviceRef();

	/////////////////////////////////
	// Shader
	/////////////////////////////////

	Ref<VulkanShader> shader = DynamicPointerCast<VulkanShader>(specs.shader);

	VkShaderModule vertShaderModule = shader->GetModule(VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule fragShaderModule = shader->GetModule(VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	
	auto& vertShaderStageInfo = shaderStages.emplace_back();
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	auto& fragShaderStageInfo = shaderStages.emplace_back();
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	/////////////////////////////////
	// Vertex Input
	/////////////////////////////////

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;

	std::vector<VkVertexInputBindingDescription> bindings(specs.bindInfo.size());

	for (uint32_t k = 0; k < bindings.size(); k++)
	{
		bindings[k].binding = specs.bindInfo[k].binding;
		bindings[k].stride = specs.bindInfo[k].stride;

		if (specs.bindInfo[k].inputRate == VertexBindInfo::INPUT_RATE_VERTEX)
			bindings[k].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		if (specs.bindInfo[k].inputRate == VertexBindInfo::INPUT_RATE_INSTANCE)
			bindings[k].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
	}


	std::vector<VkVertexInputAttributeDescription> attributes(specs.attribInfo.size());

	for (uint32_t k = 0; k < attributes.size(); k++)
	{
		attributes[k].binding = specs.attribInfo[k].binding;
		attributes[k].location = specs.attribInfo[k].location;
		attributes[k].offset = specs.attribInfo[k].offset;

		if (specs.attribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC2)
			attributes[k].format = VK_FORMAT_R32G32_SFLOAT;
		if (specs.attribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC3)
			attributes[k].format = VK_FORMAT_R32G32B32_SFLOAT;
		if (specs.attribInfo[k].format == VertexAttribInfo::ATTRIB_FORMAT_VEC4)
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
	inputAssembly.topology = GetVulkanPipelineTopology(specs.topology);
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	/////////////////////////////////
	// Rasterizer
	/////////////////////////////////

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.lineWidth = 1.f;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.depthBiasClamp = VK_FALSE;
	rasterizer.polygonMode = GetVulkanPipelinePolygonMode(specs.polygon);
	rasterizer.cullMode = GetVulkanPipelineCullMode(specs.cull);
	rasterizer.frontFace = GetVulkanPipelineFrontFaceType(specs.frontface);

	/////////////////////////////////
	// MultiSampling
	/////////////////////////////////

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = specs.multiSampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;

	/////////////////////////////////
    // Blending
    /////////////////////////////////

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(renderPass.m_colorAttachmentCount);

	for (auto& at : colorBlendAttachments)
	{
		at.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		at.blendEnable = VK_FALSE;
	}

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = (uint32_t)colorBlendAttachments.size();
	colorBlending.pAttachments = colorBlendAttachments.data();
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
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
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

	VkGraphicsPipelineCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	Info.pNext = nullptr;
	Info.basePipelineHandle = VK_NULL_HANDLE;
	Info.subpass = 0;
	Info.renderPass = renderPass.Get();

	Info.pStages = shaderStages.data();
	Info.stageCount = (uint32_t)shaderStages.size();
	Info.layout = shader->GetPipelineLayout();

	Info.pDynamicState = &dynamicState;
	Info.pDepthStencilState = &depthStencilState;
	Info.pColorBlendState = &colorBlending;
	Info.pMultisampleState = &multisampling;
	Info.pRasterizationState = &rasterizer;
	Info.pViewportState = &viewportState;
	Info.pInputAssemblyState = &inputAssembly;
	Info.pVertexInputState = &vertexInputInfo;

	_VKR = vkCreateGraphicsPipelines(device->Get(), nullptr, 1, &Info, nullptr, &m_Handle);
	CHECK_LOG_VKR;
}

void Nexus::VulkanGraphicsRenderPipeline::Destroy(VkDevice device)
{
	vkDestroyPipeline(device, m_Handle, nullptr);
}
