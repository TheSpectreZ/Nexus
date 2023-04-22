#pragma once
#include "Renderer/TransferCommandQueue.h"
#include "VkAssert.h"

#include "VkBuffer.h"

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
	private:
		VkDevice m_device;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
		VkCommandBufferBeginInfo m_CommandBufferBeginInfo;

		VkQueue m_transferQueue;
		VkSubmitInfo m_SubmitInfo;

		struct TransferData
		{
			std::vector<Ref<VulkanStaticBuffer>> m_StaticBuffers;

			bool empty()
			{
				return m_StaticBuffers.empty();
			}

			void clear();
		} m_TransferData;

		void Transfer();
	};
}
