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
}

Nexus::VulkanSwapchain::~VulkanSwapchain()
{

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
}

void Nexus::VulkanSwapchain::Shut()
{
	for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
	{
		vkDestroyImageView(m_device, m_SwapchainImages[i].view, nullptr);
	}
	NEXUS_LOG_TRACE("Vulkan Swapchain Image Attachment Destroyed: {0}", m_SwapchainImages.size());

	vkDestroySwapchainKHR(m_device, m_Swapchain, nullptr);
	NEXUS_LOG_INFO("Vulkan Swapchain Destroyed");
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
