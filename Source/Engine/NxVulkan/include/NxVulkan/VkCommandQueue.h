#pragma once
#include "NxGraphics/CommandQueue.h"
#include "VkContext.h"
#include "VkBuffer.h"
#include "VkTexture.h"
#include "VkFramebuffer.h"
#include "VkPipeline.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanCommandQueue : public CommandQueue
	{
		static VulkanCommandQueue* s_Instance;
	public:
		static VulkanCommandQueue* Get() { return s_Instance; }
		
		VulkanCommandQueue(std::function<void()> resizeCallback);
		~VulkanCommandQueue() = default;

		VkCommandBuffer GetCurrentCommandBuffer() { return m_RenderCommandBuffer[m_FrameIndex]; }
		uint32_t GetFrameIndex() override { return m_FrameIndex; }

		void Initialize() override;
		void Shutdown() override;

		void StartComputeQueue() override;
		void FlushComputeQueue() override;

		void BeginRenderQueue() override;
		void EndRenderQueue() override;
		void FlushRenderQueue() override;

		void FlushTransferQueue() override;

		void BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer) override;
		void EndRenderPass() override;

		void BindShaderResourceHeap(Ref<Shader> shader, ResourceHeapHandle handle,PipelineBindPoint bind) override;
		void BindPipeline(Ref<Pipeline> pipeline) override;

		void SetScissor(Scissor scissor) override;
		void SetViewport(Viewport viewport) override;
		void SetPushConstant(Ref<Shader> shader, PipelineBindPoint bindPoint, ShaderStage stage, void* data) override;

		void TransferBufferToGPU(VulkanBuffer* buffer);

		void TransferTextureToGPU(VulkanTexture* texture);
		void TransferTextureToGPU_Now(VulkanTexture* texture);

		void TransitionTextureLayout_Now(VulkanTexture* texture, VkImageLayout layout, VkAccessFlags flags, VkPipelineStageFlags stage, PipelineBindPoint bindPoint);

		void BindVertexBuffer(Ref<Buffer> buffer) override;
		void BindIndexBuffer(Ref<Buffer> buffer) override;
		void DrawIndices(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance) override;

		void Dispatch(uint32_t xWorkers, uint32_t yWorkers, uint32_t zWorkers);
	private:
		std::function<void()> m_ResizeCallback;

		uint32_t m_FrameIndex, m_ImageIndex, m_ImageCount;
		uint32_t m_RenderQueueIndex, m_TransferQueueIndex,m_ComputeQueueIndex;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;

		Ref<VulkanDevice> m_Device;
		VkSwapchainKHR m_Swapchain;

		VkQueue m_RenderQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;
		VkQueue m_ComputeQueue;
		
		VkCommandPool m_RenderCommandPool, m_TransferCommandPool, m_ComputeCommandPool;
		std::vector<VkCommandBuffer> m_RenderCommandBuffer, m_TransferCommandBuffer, m_ComputeCommandBuffer;

		VkCommandBufferBeginInfo m_CmdBeginInfo;

		VkPipelineStageFlags m_RenderWaitStageFlag;
		VkSubmitInfo m_RenderSubmitInfo, m_SubmitInfo;
		VkPresentInfoKHR m_PresentInfo;

		std::vector<VkFence> m_RenderFences;
		std::vector<VkSemaphore> m_ImageAvailableSemaphore, m_RenderFinishedSemaphore;
	
		struct Transferdata
		{
			std::vector<VulkanBuffer*> m_Buffer;
			std::vector<VulkanTexture*> m_Textures;

			bool Empty()
			{
				return (m_Textures.empty()) && m_Buffer.empty();
			}
			void Clear();
		} m_TransferData;
	};
}
