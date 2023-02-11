#pragma once
#include "vulkan/vulkan.h"
#include "Graphics/EngineSpecification.h"

#include "Graphics/Renderpass.h"
#include "Graphics/Framebuffer.h"

#include <vector>
#include <functional>


#define NX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Presenter
		{
			static Presenter* s_Instance;
		public:
			static void Init(const EngineSpecification& specs);
			static void Shut();

			static void StartFrame();
			static void EndFrame();

			static void BeginRenderpass(VkCommandBuffer cmdbuffer, const Renderpass& pass, const Framebuffer& buffer, std::vector<VkClearValue> values);
			static void EndRenderpass(VkCommandBuffer buffer);

			static void SetViewportAndScissor(VkViewport* pViewport, uint32_t viewportCount, VkRect2D* pScissor, uint32_t scissorCount);

			static void WaitForDevice();

			static VkImageView GetImageView(uint32_t i) { return s_Instance->m_ImageViews[i]; }

			static VkCommandBuffer GetCommandBuffer() { return s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame]; }
			static uint32_t GetFrameIndex() { return s_Instance->s_CurrentFrame; }
			
			static VkExtent2D GetImageExtent() { return s_Instance->m_SwapchainExtent; }
			static VkFormat GetImageFormat() { return s_Instance->m_SwapchainImageFormat; }

			static VkSampleCountFlagBits GetImageMaxSamples();
			static uint32_t GetMaxImageCount() { return s_Instance->s_FramesInFlight; }
				 
			static std::function<void()> s_RebootCallback;
		private:
			void Create();
			void Destroy(); 

			void ReBuild();

			static EngineSpecification s_Specs;

			uint32_t s_FramesInFlight, s_CurrentFrame,s_ImageIndex;

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