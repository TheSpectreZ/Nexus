#include "nxpch.h"
#include "VkSwapchain.h"
#include "VkContext.h"
#include "VkRenderCommandQueue.h"
#include "Core/Application.h"

Nexus::VulkanSwapchain::VulkanSwapchain()
{
	m_window = &Application::Get()->GetWindow();
	VkPhysicalDevice gpu = VulkanContext::Get()->GetPhysicalDeviceRef()->Get();
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	
	m_surface = VulkanContext::Get()->GetSurface();
	m_device = device->Get();

	m_CurrentFrame = 0;
	m_CurrentRenderCommandBuffer = nullptr;
	m_Swapchain = nullptr;
	
	// Clear Values
	{
		m_SwapchainImageClearValues.resize(2);

		m_SwapchainImageClearValues[0] = { {0.5f,0.8f,0.6f} };
		m_SwapchainImageClearValues[1] = { {1.f,0.f} };

		m_ImGuiImageClearValues.resize(1);
		m_ImGuiImageClearValues[0] = { {0.5f,0.8f,0.6f} };
	}

	// Swapchain Format, Mode and Extent
	{
		uint32_t size = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, m_surface, &size, nullptr);
		std::vector<VkSurfaceFormatKHR> availableFormats(size);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, m_surface, &size, availableFormats.data());

		size = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, m_surface, &size, nullptr);
		std::vector<VkPresentModeKHR> availableModes(size);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, m_surface, &size, availableModes.data());

		m_format = availableFormats[0];
		for (auto& form : availableFormats)
		{
			if (form.format == VK_FORMAT_B8G8R8A8_UNORM && form.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_format = form;
				break;
			}
		}

		m_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (auto& m : availableModes)
		{
			if (m == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_mode = m;
				break;
			}
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, m_surface, &m_cap);

		m_MinImageCount = m_cap.minImageCount + 1;
		if (m_cap.maxImageCount > 0 && m_MinImageCount > m_cap.maxImageCount)
		{
			m_MinImageCount = m_cap.maxImageCount;
		}

		NEXUS_LOG_WARN("Vulkan Min Image Count: {0}", m_MinImageCount);

	}

	// Swapchain Renderpass
	{
		// Color attachment
		std::vector<VkAttachmentDescription> Attachments;

		auto& colorAttachmentDesc = Attachments.emplace_back();
		colorAttachmentDesc.format = m_format.format;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto& depthAttachmentDesc = Attachments.emplace_back();
		depthAttachmentDesc.format = VulkanContext::Get()->GetPhysicalDeviceRef()->GetDepthFormat();
		depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentDesc.flags = 0;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)Attachments.size();
		renderPassInfo.pAttachments = Attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		_VKR = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_SwapchainRenderpass);
		CHECK_HANDLE(m_SwapchainRenderpass, VkRenderPass);
		NEXUS_LOG_INFO("Vulkan Swapchain Renderpass Created");
	}

	{
		std::vector<VkAttachmentDescription> Attachments;

		auto& colorAttachmentDesc = Attachments.emplace_back();
		colorAttachmentDesc.format = m_format.format;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)Attachments.size();
		renderPassInfo.pAttachments = Attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		_VKR = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_ImGuiRenderpass);
		CHECK_HANDLE(m_ImGuiRenderpass, VkRenderPass);
		NEXUS_LOG_INFO("Vulkan Swapchain Renderpass Created");
	}

}

Nexus::VulkanSwapchain::~VulkanSwapchain()
{
	vkDestroyRenderPass(m_device, m_ImGuiRenderpass, nullptr);
	NEXUS_LOG_TRACE("Vulkan ImGui Renderpass Destroyed");
	
	vkDestroyRenderPass(m_device, m_SwapchainRenderpass, nullptr);
	NEXUS_LOG_TRACE("Vulkan Swapchain Renderpass Destroyed");
}

void Nexus::VulkanSwapchain::Init()
{
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanContext::Get()->GetPhysicalDeviceRef()->Get(), m_surface, &m_cap);
		if (m_cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_extent = m_cap.currentExtent;
		}
		else
		{
			m_extent = { (uint32_t)m_window->width,(uint32_t)m_window->height };

			m_extent.width = std::clamp(m_extent.width, m_cap.currentExtent.width, m_cap.maxImageExtent.width);
			m_extent.height = std::clamp(m_extent.height, m_cap.currentExtent.height, m_cap.maxImageExtent.height);
		}
	}

	// Swapchain
	{
		VkSwapchainCreateInfoKHR Info{};
		Info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		Info.pNext = nullptr;
		Info.surface = m_surface;
		Info.flags = 0;
		Info.queueFamilyIndexCount = 0;
		Info.pQueueFamilyIndices = nullptr;
		Info.oldSwapchain = VK_NULL_HANDLE;
		Info.minImageCount = m_MinImageCount;
		Info.imageFormat = m_format.format;
		Info.imageColorSpace = m_format.colorSpace;
		Info.imageExtent = m_extent;
		Info.presentMode = m_mode;
		Info.preTransform = m_cap.currentTransform;
		Info.imageArrayLayers = 1;
		Info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		Info.clipped = VK_TRUE;
		Info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (m_cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			Info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (m_cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			Info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		_VKR = vkCreateSwapchainKHR(m_device, &Info, nullptr, &m_Swapchain);
		CHECK_HANDLE(m_Swapchain, VkSwapchainKHR);
		NEXUS_LOG_INFO("Vulkan Swapchain Created");
	}

	// Swapchain Images
	{
		uint32_t count;
		vkGetSwapchainImagesKHR(m_device, m_Swapchain, &count, nullptr);
		std::vector<VkImage> AvailableImages(count);
		vkGetSwapchainImagesKHR(m_device, m_Swapchain, &count, AvailableImages.data());
		
		NEXUS_LOG_TRACE("Vulkan Swapchain Images Acquired: {0}", count);

		m_SwapchainImages.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			m_SwapchainImages[i].vma = nullptr;
			m_SwapchainImages[i].image = AvailableImages[i];

			VkImageViewCreateInfo Info = {};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			Info.pNext = NULL;
			Info.format = m_format.format;
			Info.image = AvailableImages[i];
			Info.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Info.subresourceRange.baseMipLevel = 0;
			Info.subresourceRange.levelCount = 1;
			Info.subresourceRange.baseArrayLayer = 0;
			Info.subresourceRange.layerCount = 1;
			Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			Info.flags = 0;

			_VKR = vkCreateImageView(m_device, &Info, nullptr, &m_SwapchainImages[i].view);
			CHECK_LOG_VKR
		}

		NEXUS_LOG_TRACE("Vulkan Swapchain Image Attachment Created: {0}", count);
	}

	// Depth Image
	{
		VkFormat depthFormat = VulkanContext::Get()->GetPhysicalDeviceRef()->GetDepthFormat();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext = nullptr;
		imageInfo.arrayLayers = 1;
		imageInfo.extent = { m_extent.width,m_extent.height,1 };
		imageInfo.format = depthFormat;
		imageInfo.flags = 0;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.mipLevels = 1;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		
		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		
		_VKR = vmaCreateImage(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), &imageInfo, &allocInfo, &m_DepthImage.image, &m_DepthImage.vma, nullptr);
		CHECK_LOG_VKR

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = nullptr;
		viewInfo.image = m_DepthImage.image;
		viewInfo.flags = 0;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = 1;

		_VKR = vkCreateImageView(m_device, &viewInfo, nullptr, &m_DepthImage.view);
		CHECK_LOG_VKR
		
		NEXUS_LOG_TRACE("Vulkan Swapchain Depth Attachment Created");
	}

	// Swapchain Framebuffers
	{
		std::array<VkImageView, 2> views{};
		views[1] = m_DepthImage.view;

		VkFramebufferCreateInfo Info{}, Info2{};
		Info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.attachmentCount = (uint32_t)views.size();
		Info.pAttachments = views.data();
		Info.layers = 1;
		Info.width = m_extent.width;
		Info.height = m_extent.height;
		Info.renderPass = m_SwapchainRenderpass;

		Info2 = Info;
		Info2.renderPass = m_ImGuiRenderpass;
		Info2.attachmentCount = 1;

		m_SwapchainFramebuffers.resize(m_SwapchainImages.size());
		m_ImGuiFramebuffers.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			views[0] = m_SwapchainImages[i].view;
			
			_VKR = vkCreateFramebuffer(m_device, &Info, nullptr, &m_SwapchainFramebuffers[i]);
			CHECK_LOG_VKR;
			
			Info2.pAttachments = &m_SwapchainImages[i].view;
			_VKR = vkCreateFramebuffer(m_device, &Info2, nullptr, &m_ImGuiFramebuffers[i]);
			CHECK_LOG_VKR;
		}
		NEXUS_LOG_INFO("Vulkan ImGui Framebuffers Created: {0} | Attachment Count: {1}", m_SwapchainImages.size(), views.size());
		NEXUS_LOG_INFO("Vulkan Swapchain Framebuffers Created: {0} | Attachment Count: {1}", m_SwapchainImages.size(), views.size());
	}

}

void Nexus::VulkanSwapchain::Shut()
{
	// Swapchain Framebuffers
	for (uint32_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(m_device, m_ImGuiFramebuffers[i], nullptr);
		vkDestroyFramebuffer(m_device, m_SwapchainFramebuffers[i], nullptr);
	}
	NEXUS_LOG_TRACE("Vulkan ImGui Framebuffers Destroyed: {0}",m_ImGuiFramebuffers.size());
	NEXUS_LOG_TRACE("Vulkan Swapchain Framebuffers Destroyed: {0}",m_SwapchainFramebuffers.size());

	// Depth Attachment
	{
		vkDestroyImageView(m_device, m_DepthImage.view, nullptr);
		vmaDestroyImage(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_DepthImage.image, m_DepthImage.vma);
	}
	NEXUS_LOG_TRACE("Vulkan Swapchain Depth Attachment Destroyed");

	// Swapchain Attachment
	for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
	{
		vkDestroyImageView(m_device, m_SwapchainImages[i].view, nullptr);
	}
	NEXUS_LOG_TRACE("Vulkan Swapchain Image Attachment Destroyed: {0}", m_SwapchainImages.size());

	vkDestroySwapchainKHR(m_device, m_Swapchain, nullptr);
	NEXUS_LOG_INFO("Vulkan Swapchain Destroyed");
}

void Nexus::VulkanSwapchain::BeginSwapchainPass()
{
	Ref<VulkanRenderCommandQueue> queue = DynamicPointerCast<VulkanRenderCommandQueue>(Renderer::GetRenderCommandQueue());
	
	m_CurrentRenderCommandBuffer = queue->GetCurrentCommandBuffer();
	m_CurrentFrame = queue->GetFrameIndex();

	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = m_SwapchainRenderpass;
	Info.framebuffer = m_SwapchainFramebuffers[m_CurrentFrame];
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = m_extent;
	Info.clearValueCount = (uint32_t)m_SwapchainImageClearValues.size();
	Info.pClearValues = m_SwapchainImageClearValues.data();

	vkCmdBeginRenderPass(m_CurrentRenderCommandBuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::VulkanSwapchain::BeginImGuiPass()
{	
	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = m_ImGuiRenderpass;
	Info.framebuffer = m_ImGuiFramebuffers[m_CurrentFrame];
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = m_extent;
	Info.clearValueCount = (uint32_t)m_ImGuiImageClearValues.size();
	Info.pClearValues = m_ImGuiImageClearValues.data();

	vkCmdBeginRenderPass(m_CurrentRenderCommandBuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::VulkanSwapchain::EndPass()
{
	vkCmdEndRenderPass(m_CurrentRenderCommandBuffer);
}

Nexus::Extent Nexus::VulkanSwapchain::GetExtent()
{
	return { m_extent.width,m_extent.height };
}

void Nexus::VulkanSwapchain::ReCreate()
{
	VulkanContext::Get()->GetDeviceRef()->Wait();

	Shut();
	Init();
}
