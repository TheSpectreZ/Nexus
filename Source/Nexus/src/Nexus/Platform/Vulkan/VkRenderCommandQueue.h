#pragma once
#include "Renderer/RenderCommandQueue.h"	
#include "VkAssert.h"

namespace Nexus
{

	class VulkanRenderCommandQueue : public RenderCommandQueue
	{
	public:
		VulkanRenderCommandQueue();
		~VulkanRenderCommandQueue() override;

		void Begin() override;
		void End() override;
		void Flush() override;

		uint32_t GetFrameIndex() { return m_FrameIndex; }
		VkCommandBuffer GetCurrentCommandBuffer() { return m_CommandBuffer[m_FrameIndex]; }
	private:
		void Render();

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffer;
		VkCommandBufferBeginInfo m_CommandBufferBeginInfo;

		VkDevice m_device;
		VkSwapchainKHR m_swapchain;
		
		VkQueue m_renderQueue, m_presentQueue;

		VkPipelineStageFlags m_waitStages[1];
		VkSubmitInfo m_SubmitInfo;
		VkPresentInfoKHR m_PresentInfo;
		
		uint32_t m_ImageCount;
		uint32_t m_FrameIndex;
		uint32_t m_ImageIndex;

		struct SyncHandles
		{
			VkFence fence;
			VkSemaphore renderFinished, imageAvailable;
		};
		std::vector<SyncHandles> m_SyncHandles;
	};

}