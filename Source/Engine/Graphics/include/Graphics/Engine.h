#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"
#include "EngineSpecification.h"

#include <functional>

#define NX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
VK_DEFINE_HANDLE(VmaAllocator);

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Engine
		{
		public:
			inline static Engine& Get()
			{
				static Engine Instance;
				return Instance;
			}

			// Function Calls
			void Initialize(const EngineSpecification& Specs, std::function<void(uint32_t, uint32_t)> ResizeCallback);
			void Shutdown();
			void WaitForDevice();
			
			VkCommandBuffer StartFrame();
			void EndFrame();

			// Getters
			VkInstance GetInstance();
			VkPhysicalDevice GetGpuHandle();
			VkDevice GetDevice();
			VkSurfaceKHR GetSurface();
			VkQueue GetGraphicsQueue();
			VkQueue GetPresentQueue();
			VkSwapchainKHR GetSwapchain();
			VmaAllocator GetAllocator();
			VkSampleCountFlagBits GetMaxSampleCount();
			VkCommandBuffer GetCurrentRenderCommandBuffer();
			VkImageView GetSwapChainImageView();
			VkExtent2D GetSwapchainImageExtent();
			VkFormat GetSwapchainImageFormat();
			uint32_t GetMaxSwapchainImageCount();
			uint32_t GetCurrentFrameIndex();
		};
	}
}


