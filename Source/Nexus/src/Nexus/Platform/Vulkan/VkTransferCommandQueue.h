#pragma once
#include "Renderer/TransferCommandQueue.h"
#include "VkAssert.h"

#include "VkBuffer.h"
#include "VkTexture.h"

namespace Nexus 
{
	class VulkanTransferCommandQueue : public TransferCommandQueue
	{
	public:
		VulkanTransferCommandQueue();
		~VulkanTransferCommandQueue() override;

		void Flush() override;

		VkCommandBuffer GetCurrentCommandBuffer() { return m_CommandBuffer; }
		void PushStaticBuffer(Ref<VulkanStaticBuffer> buffer);
		void PushTexture(Ref<VulkanTexture> texture);
	private:
		VkDevice m_device;
		uint32_t m_TransferQueueIndex, m_RenderQueueIndex;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
		VkCommandBufferBeginInfo m_CommandBufferBeginInfo;

		VkQueue m_transferQueue;
		VkSubmitInfo m_SubmitInfo;

		struct TransferData
		{
			std::vector<Ref<VulkanStaticBuffer>> m_StaticBuffers;
			std::vector<Ref<VulkanTexture>> m_Textures;

			bool empty()
			{
				return m_StaticBuffers.empty() && m_Textures.empty();
			}

			void clear();
		} m_TransferData;

		void Transfer();
		void DoImageBarriers();
	};
}
