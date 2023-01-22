#pragma once
#include "vulkan/vulkan.h"
#include "EngineSpecification.h"

#include <vector>
#include <functional>

namespace Nexus
{
	namespace Graphics
	{
		class Presenter
		{
			static Presenter* s_Instance;
		public:
			static void Init(const EngineSpecification& specs);
			static void Shut();

			static void StartFrame();
			static void EndFrame();

			static VkCommandBuffer GetCommandBuffer() { return s_Instance->m_CommandBuffers[s_CurrentFrame]; }

			static std::function<void(uint32_t, uint32_t)> s_RebootCallback;
		private:
			void Create();
			void Destroy(); 

			static EngineSpecification s_Specs;

			VkSwapchainKHR m_Swapchain;
			VkFormat m_SwapchainImageFormat;
			VkExtent2D m_SwapchainExtent;

			std::vector<VkImage> m_Images;
			std::vector<VkImageView> m_ImageViews;

			static uint32_t s_FramesInFlight, s_CurrentFrame,s_ImageIndex;

			std::vector<VkFence> m_Fences;
			std::pair<std::vector<VkSemaphore>, std::vector<VkSemaphore>> m_Semaphores;
		
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
		};
	}
}