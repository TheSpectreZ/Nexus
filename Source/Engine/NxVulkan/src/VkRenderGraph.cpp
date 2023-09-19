#include "NxVulkan/VkRenderGraph.h"

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
	for (auto& [name, specs] : m_RenderTargets)
		m_Attachments[name].Create(specs);

	for (auto& [name, specs] : m_GPUpasses)
		m_Passes[name].Create(specs, this);

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

			VkExtent3D extent{ 0,0,1 };
			extent.width = specs.extent.width > 0 ? specs.extent.width : sExtent.width;
			extent.height = specs.extent.height > 0 ? specs.extent.height : sExtent.height;

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

	vkCreateRenderPass(device->Get(), &Info, nullptr, &m_Handle);
}

void Nexus::VulkanRenderGraphPass::Destroy(VkDevice device)
{
	vkDestroyRenderPass(device, m_Handle, nullptr);
}
