#include "Graphics/Presenter.h"
#include "vkAssert.h"
#include "Graphics/Backend.h"

Nexus::Graphics::Presenter* Nexus::Graphics::Presenter::s_Instance = nullptr;

void Nexus::Graphics::Presenter::Init(const EngineSpecification& specs)
{
	s_Instance = new Presenter();

	// Swapchain
	{
		VkSwapchainCreateInfoKHR Info{};
		Info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		Info.pNext = nullptr;
		Info.surface = Backend::GetSurface();
		
		QueueIndexFamilies Fam = Graphics::GetQueueIndexFamilies(Backend::GetPhysicalDevice(), Backend::GetSurface());
	
		if (auto Fam = GetQueueIndexFamilies(Backend::GetPhysicalDevice(), Backend::GetSurface()); 
			Fam.front() != Fam.back())
		{
			uint32_t Indices[] = { Fam.front().value(),Fam.back().value() };
			Info.queueFamilyIndexCount = 2;
			Info.pQueueFamilyIndices = Indices;
			Info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else
		{
			Info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			Info.queueFamilyIndexCount = 0;
			Info.pQueueFamilyIndices = nullptr;
		}

		// format, colorspace, mode, capabilities, minImage
		{
			VkSurfaceCapabilitiesKHR cap;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &cap);
		
			uint32_t size = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, nullptr);
			std::vector<VkSurfaceFormatKHR> availableFormats(size);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, availableFormats.data());
			
			size = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, nullptr);
			std::vector<VkPresentModeKHR> availableModes(size);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, availableModes.data());

			VkSurfaceFormatKHR format = availableFormats[0];
			for (auto& form : availableFormats)
			{
				if (form.format == VK_FORMAT_B8G8R8A8_UNORM && form.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					format = form;
					break;
				}
			}

			VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
			for (auto& m : availableModes)
			{
				if (m == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					mode = m;
					break;
				}
			}

			VkExtent2D extent;
			if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				extent = cap.currentExtent;
			}
			else
			{
				extent = { (uint32_t)specs.targetWindow.width,(uint32_t)specs.targetWindow.height };

				extent.width = std::clamp(extent.width, cap.currentExtent.width, cap.maxImageExtent.width);
				extent.height = std::clamp(extent.height, cap.currentExtent.height, cap.maxImageExtent.height);
			}

			uint32_t MinImageCount = cap.minImageCount + 1;
			if (cap.maxImageCount > 0 && MinImageCount > cap.maxImageCount)
			{
				MinImageCount = cap.maxImageCount;
			}

			Info.minImageCount = MinImageCount;
			Info.imageFormat = format.format;
			Info.imageColorSpace = format.colorSpace;
			Info.imageExtent = extent;
			Info.presentMode = mode;
			Info.preTransform = cap.currentTransform;
		}

		Info.imageArrayLayers = 1;
		Info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		Info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		Info.clipped = VK_TRUE;
		Info.oldSwapchain = VK_NULL_HANDLE;

		_VKR = vkCreateSwapchainKHR(Backend::GetDevice(), &Info, nullptr, &s_Instance->m_Swapchain);
		CHECK_HANDLE(s_Instance->m_Swapchain, VkSwapchainKHR);
		NEXUS_LOG_WARN("Vulkan Swapchain Created");

		s_Instance->m_SwapchainExtent = Info.imageExtent;
		s_Instance->m_SwapchainImageFormat = Info.imageFormat;
	}

	// Swapchain Images
	{
		uint32_t count;
		vkGetSwapchainImagesKHR(Backend::GetDevice(), s_Instance->m_Swapchain, &count, nullptr);
		s_Instance->m_Images.clear();
		s_Instance->m_Images.resize(count);
		vkGetSwapchainImagesKHR(Backend::GetDevice(), s_Instance->m_Swapchain, &count, s_Instance->m_Images.data());
		
		for (auto& i : s_Instance->m_Images)
		{
			CHECK_HANDLE(i, VkImage);
		}
		NEXUS_LOG_TRACE("Vulkan Swapchain Images[{0}] Acquired", count);
	}

	// Swapchain Image Views
	{
		uint32_t count = s_Instance->m_Images.size();

		s_Instance->m_ImageViews.clear();
		s_Instance->m_ImageViews.resize(count);

		for (uint32_t i = 0; i < count; i++)
		{
			VkImageViewCreateInfo Info{};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			Info.pNext = nullptr;
			Info.flags = 0;
			Info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.format = s_Instance->m_SwapchainImageFormat;
			Info.image = s_Instance->m_Images[i];
			Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Info.subresourceRange.baseArrayLayer = 0;
			Info.subresourceRange.baseMipLevel = 0;
			Info.subresourceRange.layerCount = 1;
			Info.subresourceRange.levelCount = 1;

			_VKR = vkCreateImageView(Backend::GetDevice(), &Info, nullptr, &s_Instance->m_ImageViews[i]);
			CHECK_HANDLE(s_Instance->m_ImageViews[i], VkImageView);
		}
		NEXUS_LOG_TRACE("Vulkan Swapchain Image Views[{0}] Created", count);
	}
}

void Nexus::Graphics::Presenter::Shut()
{
	for (auto& view : s_Instance->m_ImageViews)
	{
		vkDestroyImageView(Backend::GetDevice(), view, nullptr);
	}

	vkDestroySwapchainKHR(Backend::GetDevice(), s_Instance->m_Swapchain, nullptr);

	NEXUS_LOG_WARN("Vulkan Presenter Shut");

	delete s_Instance;
}
