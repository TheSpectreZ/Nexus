#include "nxpch.h"
#include "VkTransferCommandQueue.h"
#include "VkContext.h"
#include "VkSwapchain.h"

Nexus::VulkanTransferCommandQueue::VulkanTransferCommandQueue()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	m_device = device->Get();
	m_transferQueue = device->GetTransferQueue();

	VkCommandPoolCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	Info.queueFamilyIndex = device->GetQueueFamilyIndices().Transfer;

	_VKR = vkCreateCommandPool(m_device, &Info, nullptr, &m_CommandPool);
	CHECK_HANDLE(m_CommandPool, VkCommandPool);

	VkCommandBufferAllocateInfo aInfo{};
	aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	aInfo.pNext = nullptr;
	aInfo.commandPool = m_CommandPool;
	aInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	aInfo.commandBufferCount = 1;

	_VKR = vkAllocateCommandBuffers(m_device, &aInfo, &m_CommandBuffer);
	CHECK_LOG_VKR;

	m_CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	m_CommandBufferBeginInfo.pNext = nullptr;
	m_CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	m_CommandBufferBeginInfo.flags = 0;

	m_SubmitInfo = {};
	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_SubmitInfo.pNext = nullptr;
	m_SubmitInfo.pCommandBuffers = &m_CommandBuffer;
	m_SubmitInfo.commandBufferCount = 1;
	
	NEXUS_LOG_TRACE("Vulkan Transfer Command Queue Created");
}

Nexus::VulkanTransferCommandQueue::~VulkanTransferCommandQueue()
{
	vkDestroyCommandPool(m_device, m_CommandPool, nullptr);
	NEXUS_LOG_TRACE("Vulkan Transfer Command Queue Destroyed");
}

void Nexus::VulkanTransferCommandQueue::Flush()
{
	if (!m_TransferData.empty())
	{
		vkBeginCommandBuffer(m_CommandBuffer, &m_CommandBufferBeginInfo);
		Transfer();
		vkEndCommandBuffer(m_CommandBuffer);
		
		_VKR = vkQueueSubmit(m_transferQueue, 1, &m_SubmitInfo, nullptr);
		vkQueueWaitIdle(m_transferQueue);
		m_TransferData.clear();
	}
}

void Nexus::VulkanTransferCommandQueue::PushStaticBuffer(Ref<VulkanStaticBuffer> buffer)
{
	m_TransferData.m_StaticBuffers.emplace_back(buffer);
}

void Nexus::VulkanTransferCommandQueue::Transfer()
{
	// Static Buffers
	for (auto& buffer : m_TransferData.m_StaticBuffers)
	{
		VkBufferCopy copy{};
		copy.size = buffer->m_size;
		copy.dstOffset = 0;
		copy.srcOffset = 0;

		vkCmdCopyBuffer(m_CommandBuffer, buffer->m_StagingBuff, buffer->m_buffer, 1, &copy);
	}
}

void Nexus::VulkanTransferCommandQueue::TransferData::clear()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	for (auto& b : m_StaticBuffers)
	{
		vmaDestroyBuffer(device->GetAllocator(), b->m_StagingBuff, b->m_StagingAlloc);
		b->m_StagingBuff = nullptr;
	}
	m_StaticBuffers.clear();
}
