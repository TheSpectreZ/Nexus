#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/Swapchain.h"
#include "NxVulkan/VkAssert.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanSwapchain : public Swapchain
	{
		static VulkanSwapchain* s_Instance;
	public:
		VulkanSwapchain(Window* window);
		~VulkanSwapchain() override;
		
		static VulkanSwapchain* Get() { return s_Instance; }

		void ReCreate();
	
		VkSwapchainKHR GetHandle() { return m_Swapchain; }	
		VkExtent2D GetImageExtent() { return m_extent; }
		VkFormat GetImageFormat();
		uint32_t GetImageCount() override { return (uint32_t)m_SwapchainImages.size(); }
		VkImageView GetImageView(uint32_t index) 
		{ 
			NEXUS_ASSERT((index>=m_SwapchainImages.size()),"Swapchain Image View Index out of Bounds"); 
			return m_SwapchainImages[index].view; 
		}

		void Initialize() override;
		void Shutdown() override;

		Extent GetExtent() override;
	private:
		VkDevice m_device;
		VkSurfaceKHR m_surface;
		Window* m_window;

		VkSwapchainKHR m_Swapchain;
		
		uint32_t m_CurrentFrame;
		
		VkSurfaceCapabilitiesKHR m_cap;
		VkSurfaceFormatKHR m_format;
		VkPresentModeKHR m_mode;
		VkExtent2D m_extent;
		uint32_t m_MinImageCount;

		struct SwapchainImage
		{
			VkImage image = nullptr;
			VkImageView view = nullptr;
		};
		std::vector<SwapchainImage> m_SwapchainImages;
	};
}
