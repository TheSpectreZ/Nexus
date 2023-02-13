#pragma once
#include "vulkan/vulkan.h"
#include "Graphics/EngineSpecification.h"

#include "Graphics/Renderpass.h"
#include "Graphics/Framebuffer.h"

#include <vector>
#include <functional>

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Presenter
		{
			friend class Engine;
		public:
			inline static Presenter& Get()
			{
				static Presenter instance;
				return instance;
			}

			void Init(const EngineSpecification& specs);
			void Shut();

			void StartFrame();
			void EndFrame();

			std::function<void(uint32_t, uint32_t)> WindowResizeCallbackFnc;	 
		private:
			void Create();
			void Destroy(); 

			void ReBuild();
		
			static uint32_t s_FramesInFlight;
			static uint32_t s_CurrentFrame;
			static uint32_t s_ImageIndex;
			
			Platform::Window* m_window;

			VkSwapchainKHR m_Swapchain;
			VkFormat m_SwapchainImageFormat;
			VkExtent2D m_SwapchainExtent;

			std::vector<VkImage> m_Images;
			std::vector<VkImageView> m_ImageViews;

			std::vector<VkFence> m_Fences;
			std::pair<std::vector<VkSemaphore>, std::vector<VkSemaphore>> m_Semaphores;
		
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
		};
	}
}