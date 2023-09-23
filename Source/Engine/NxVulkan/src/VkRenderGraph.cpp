#include "NxVulkan/VkRenderGraph.h"
#include "NxVulkan/VkTexture.h"

#include "NxCore/UUID.h"

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
	case Nexus::TextureFormat::RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;
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

static VkVertexInputRate GetVulkanVertexInputRate(Nexus::RenderPipelineVertexInputRate rate)
{
	switch (rate)
	{
	case Nexus::RenderPipelineVertexInputRate::PerVertex:
		return VK_VERTEX_INPUT_RATE_VERTEX;
	case Nexus::RenderPipelineVertexInputRate::PerInstance:
		return VK_VERTEX_INPUT_RATE_INSTANCE;
	default:
		return VK_VERTEX_INPUT_RATE_MAX_ENUM;
	}
}

static VkFormat GetVulkanVertexAttribFormat(Nexus::RenderPipelineVertexAttribFormat format)
{
	switch (format)
	{
	case Nexus::RenderPipelineVertexAttribFormat::Vec2:
		return VK_FORMAT_R32G32_SFLOAT;
	case Nexus::RenderPipelineVertexAttribFormat::Vec3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case Nexus::RenderPipelineVertexAttribFormat::Vec4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	default:
		return VK_FORMAT_MAX_ENUM;
	}
}

static VkDescriptorType GetVulkanShaderResourceType(Nexus::ShaderResourceType Type)
{
	switch (Type)
	{
	case Nexus::ShaderResourceType::Uniform:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case Nexus::ShaderResourceType::SampledImage:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case Nexus::ShaderResourceType::StorageImage:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	default:
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

static VkShaderStageFlags GetVulkanShaderStageFlag(Nexus::ShaderStage Stage)
{
	switch (Stage)
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

Nexus::VulkanRenderGraph::VulkanRenderGraph()
{
	auto context = VulkanContext::Get();
	m_Device = context->GetDeviceRef();
	m_pDevice = context->GetPhysicalDeviceRef();

	SamplerSpecification specs{};
	specs.ResolveHash(11122);

	m_Sampler = CreateRef<VulkanSampler>(specs);
}

Nexus::VulkanRenderGraph::~VulkanRenderGraph()
{
	for (auto& [k, v] : m_GraphicsPipelines)
	{
		v.Destroy(m_Device->Get());
		NEXUS_LOG("Vulkan", "Graphics Pipeline Destroyed: %s", k.c_str());
	}

	for (auto& [k, v] : m_Passes)
	{
		v.Destroy(m_Device->Get());
		NEXUS_LOG("Vulkan", "RenderPass Destroyed: %s", k.c_str());
	}

	for (auto& [k, v] : m_Attachments)
	{
		v.Destroy(m_Device->Get(), m_Device->GetAllocator());
		NEXUS_LOG("Vulkan", "RenderTarget Destroyed: %s", k.c_str());
	}
}

void Nexus::VulkanRenderGraph::Bake()
{
	for (auto& [name, RT] : m_RenderTargets)
	{
		m_Attachments[name].Create(RT);
		NEXUS_LOG("Vulkan", "RenderTarget Created: %s", name.c_str());
	}

	for (auto& [name, specs] : m_GPUpasses)
	{
		m_Passes[name].Create(specs, this);
		NEXUS_LOG("Vulkan", "RenderPass Created: %s", name.c_str());

		for (auto& pName : specs.getGraphicsPipelines())
		{
			m_GraphicsPipelines[pName].Create(m_GraphicsRenderPipelines[pName], m_Passes[name], this);
			NEXUS_LOG("Vulkan", "Graphics Pipeline Created: %s", pName.c_str());
		}
	}

	NEXUS_LOG("Vulkan", "RenderGraph Bake Complete");
}

void Nexus::VulkanRenderTarget::Create(const RenderTargetSpecification& specs)
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

			VkImageCreateInfo Info{};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			Info.pNext = nullptr;
			Info.extent = { sExtent.width,sExtent.height,1 };
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

void Nexus::VulkanGraphicsRenderPipeline::Create(const GraphicsRenderPipelineSpecification& specs, VulkanRenderGraphPass& renderPass, VulkanRenderGraph* graph)
{	
	auto device = VulkanContext::Get()->GetDeviceRef();
	auto gpu = VulkanContext::Get()->GetPhysicalDeviceRef();

	Ref<VulkanShader> shader = DynamicPointerCast<VulkanShader>(specs.shader);
	m_DescriptorManager.Create(shader->GetReflection());
	
	// Pipeline 
	{
	
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

		std::vector<VkVertexInputBindingDescription> bindings;
		if (specs.vertexStride)
		{
			auto& v = bindings.emplace_back();
			v.stride = specs.vertexStride;
			v.inputRate = GetVulkanVertexInputRate(specs.vertexInputRate);
		}

		std::vector<VkVertexInputAttributeDescription> attributes(specs.attribInfo.size());

		for (uint32_t k = 0; k < attributes.size(); k++)
		{
			auto& [format, location, offset] = specs.attribInfo[k];

			attributes[k].binding = 0;
			attributes[k].location = location;
			attributes[k].offset = offset;
			attributes[k].format = GetVulkanVertexAttribFormat(format);
		}

		vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)bindings.size();
		vertexInputInfo.pVertexBindingDescriptions = bindings.data();

		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
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
		Info.layout = m_DescriptorManager.GetPipelineLayout();

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

	// Descriptor Bindings
	{
		uint32_t frame = VulkanSwapchain::Get()->GetImageCount();

		std::vector<VkDescriptorImageInfo> imageinfos(frame);
		std::vector<VkWriteDescriptorSet> writes(frame);

		for (auto& [input, binding] : specs.InputInfo)
		{
			auto& RT = graph->m_Attachments[input];

			uint64_t hashID = Nexus::UUID();
			m_DescriptorManager.AllocateSet(device->Get(), 0, hashID);

			for (uint32_t i = 0; i < frame; i++)
			{
				imageinfos[i].imageView = RT.Get(i);
				imageinfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageinfos[i].sampler = graph->m_Sampler->Get();

				writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writes[i].pNext = nullptr;
				writes[i].descriptorCount = 1;
				writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				writes[i].dstArrayElement = 0;
				writes[i].dstBinding = binding;
				writes[i].pImageInfo = &imageinfos[i];
				writes[i].dstSet = m_DescriptorManager.GetSet(0, hashID, i);
			}

			vkUpdateDescriptorSets(device->Get(), (uint32_t) writes.size(), writes.data(), 0, nullptr);
		}
	}
}

void Nexus::VulkanGraphicsRenderPipeline::Destroy(VkDevice device)
{
	m_DescriptorManager.Destroy(device);
	vkDestroyPipeline(device, m_Handle, nullptr);
}

void Nexus::VulkanPipelineDescriptorManager::Create(ReflectionData& shaderRefection)
{
	auto device = VulkanContext::Get()->GetDeviceRef();
	m_FrameCount = VulkanSwapchain::Get()->GetImageCount();
	m_Reflection = shaderRefection;

	for (auto& [set, bindings] : m_Reflection.bindings)
	{
		// Descriptor Set Layout
		{
			std::vector<VkDescriptorSetLayoutBinding> vkBinds(bindings.size());

			for (uint32_t i = 0; i < bindings.size(); i++)
			{
				vkBinds[i].binding = bindings[i].bindPoint;
				vkBinds[i].descriptorCount = bindings[i].arrayCount;
				vkBinds[i].pImmutableSamplers = nullptr;
				vkBinds[i].stageFlags = GetVulkanShaderStageFlag(bindings[i].stage);
				vkBinds[i].descriptorType = GetVulkanShaderResourceType(bindings[i].type);
			}

			VkDescriptorSetLayoutCreateInfo cInfo{};
			cInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			cInfo.pNext = nullptr;
			cInfo.flags = 0;
			cInfo.bindingCount = (uint32_t)vkBinds.size();
			cInfo.pBindings = vkBinds.data();

			_VKR = vkCreateDescriptorSetLayout(device->Get(), &cInfo, nullptr, &m_Layouts[set]);
			CHECK_LOG_VKR;
		}

		MakeDescriptorPool(device->Get(), set);
		AllocateSet(device->Get(), set, 12345);
	}

	// Pipeline layout
	{
		std::vector<VkPushConstantRange> ranges;
		for (auto& [k, v] : m_Reflection.ranges)
			ranges.push_back(v);
		
		std::vector<VkDescriptorSetLayout> layouts;
		for (auto& [k, v] : m_Layouts)
			layouts.push_back(v);

		VkPipelineLayoutCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.pushConstantRangeCount = (uint32_t)ranges.size();
		Info.pPushConstantRanges = ranges.data();
		Info.setLayoutCount = (uint32_t)layouts.size();
		Info.pSetLayouts = layouts.data();
		
		_VKR = vkCreatePipelineLayout(device->Get(), &Info, nullptr, &m_PipelineLayout);
		CHECK_LOG_VKR;
	}
}

void Nexus::VulkanPipelineDescriptorManager::Destroy(VkDevice device)
{
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

	for (auto& [k, v] : m_Layouts)
		vkDestroyDescriptorSetLayout(device, v, nullptr);
}

void Nexus::VulkanPipelineDescriptorManager::MakeDescriptorPool(VkDevice device, uint32_t set)
{
	auto& pool = m_Pools[set].pools.emplace_back();
	m_Pools[set].currentID++;

	std::vector<VkDescriptorPoolSize> poolDesc;
	
	for (auto& [k, binding] : m_Reflection.bindings[set])
	{
		auto& desc = poolDesc.emplace_back();
		desc.descriptorCount = s_MaxDescriptorSetPerPoolCount;
		desc.type = GetVulkanShaderResourceType(binding.type);
	}

	VkDescriptorPoolCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = 0;
	Info.maxSets = s_MaxDescriptorSetPerPoolCount;
	Info.poolSizeCount = (uint32_t)poolDesc.size();
	Info.pPoolSizes = poolDesc.data();

	_VKR = vkCreateDescriptorPool(device, &Info, nullptr, &pool);
		
}

void Nexus::VulkanPipelineDescriptorManager::AllocateSet(VkDevice device, uint32_t set, uint64_t hashId)
{
	auto& pool = m_Pools[set];

	std::vector<VkDescriptorSetLayout> layouts(m_FrameCount);
	for (auto& l : layouts)
		l = m_Layouts[set];

	VkDescriptorSetAllocateInfo	Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.pSetLayouts = layouts.data();
	Info.descriptorSetCount = m_FrameCount;
	pool.sets[hashId].sets.resize(m_FrameCount);

	bool allocSuccess = false;

	do
	{
		Info.descriptorPool = pool.Get();
		_VKR = vkAllocateDescriptorSets(device, &Info, pool.sets[hashId].sets.data());

		if (_VKR == VK_SUCCESS)
			allocSuccess = true;
		else if(_VKR == VK_ERROR_OUT_OF_POOL_MEMORY)
			MakeDescriptorPool(device, set);
		else
		{
			NEXUS_BREAK_ASSERT(1, "Vulkan Descriptor Heap Allocation Error");
		}

	} while (!allocSuccess);
}

VkDescriptorSet& Nexus::VulkanPipelineDescriptorManager::GetSet(uint32_t set, uint64_t hashId, uint32_t frameIndex)
{
	return m_Pools[set].sets[hashId].Get(frameIndex);
}