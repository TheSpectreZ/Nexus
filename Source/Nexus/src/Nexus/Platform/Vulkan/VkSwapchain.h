#pragma once
#include "Renderer/Swapchain.h"
#include "VkAssert.h"
#include "vk_mem_alloc.h"
#include "VkDevice.h"
#include "Core/Window.h"
#include "Renderer/Renderer.h"

namespace Nexus
{
	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		void ReCreate();
		
		static Ref<VulkanSwapchain> Get() { return DynamicPointerCast<VulkanSwapchain>(Renderer::GetSwapchain()); }

		VkSwapchainKHR GetHandle() { return m_Swapchain; }
		VkRenderPass GetRenderpass() { return m_SwapchainRenderpass; }
		VkFramebuffer GetFramebuffer(uint32_t index)
		{
			NEXUS_ASSERT((index >= m_SwapchainFramebuffers.size()), "Swapchain Framebuffer Index out of Bounds");
			return m_SwapchainFramebuffers[index];
		}

		VkExtent2D GetImageExtent() { return m_extent; }
		VkFormat GetImageFormat() { return m_format.format; }
		uint32_t GetImageCount() { return (uint32_t)m_SwapchainImages.size(); }
		VkImageView GetImageView(uint32_t index) 
		{ 
			NEXUS_ASSERT((index>=m_SwapchainImages.size()),"Swapchain Image View Index out of Bounds"); 
			return m_SwapchainImages[index].view; 
		}

		VkCommandBuffer GetCurrentCommandBuffer() { return m_CurrentRenderCommandBuffer; }
		
		void Init() override;
		void Shut() override;

		void BeginRenderPass() override;
		void EndRenderPass() override;

		Extent GetExtent() override;
	private:
		VkDevice m_device;
		VkSurfaceKHR m_surface;
		Window* m_window;

		VkSwapchainKHR m_Swapchain;
		
		uint32_t m_CurrentFrame;
		VkCommandBuffer m_CurrentRenderCommandBuffer;

		VkSurfaceCapabilitiesKHR m_cap;
		VkSurfaceFormatKHR m_format;
		VkPresentModeKHR m_mode;
		VkExtent2D m_extent;
		uint32_t m_MinImageCount;

		struct SwapchainImage
		{
			VkImage image = nullptr;
			VkImageView view = nullptr;
			VmaAllocation vma = nullptr;
		};
		SwapchainImage m_DepthImage;
		std::vector<SwapchainImage> m_SwapchainImages;

		VkRenderPass m_SwapchainRenderpass;
		std::vector<VkFramebuffer> m_SwapchainFramebuffers;
		std::vector<VkClearValue> m_SwapchainImageClearValues;
	};
}
