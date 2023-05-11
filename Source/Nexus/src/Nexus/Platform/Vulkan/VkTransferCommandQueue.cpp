#include "nxpch.h"
#include "VkTransferCommandQueue.h"
#include "VkRenderCommandQueue.h"
#include "VkContext.h"
#include "VkSwapchain.h"

Nexus::VulkanTransferCommandQueue::VulkanTransferCommandQueue()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	m_device = device->Get();
	m_transferQueue = device->GetTransferQueue();

	m_TransferQueueIndex = device->GetQueueFamilyIndices().Transfer;
	m_RenderQueueIndex = device->GetQueueFamilyIndices().Graphics;

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

		DoImageBarriers();

		m_TransferData.clear();
	}
}

void Nexus::VulkanTransferCommandQueue::PushStaticBuffer(Ref<VulkanStaticBuffer> buffer)
{
	m_TransferData.m_StaticBuffers.emplace_back(buffer);
}

void Nexus::VulkanTransferCommandQueue::PushTexture(Ref<VulkanTexture> texture)
{
	m_TransferData.m_Textures.emplace_back(texture);
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
	
	for (auto& texture : m_TransferData.m_Textures)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.image = texture->m_Image;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcQueueFamilyIndex = m_TransferQueueIndex;
		barrier.dstQueueFamilyIndex = m_TransferQueueIndex;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		
		vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkBufferImageCopy copy{};

		copy.bufferOffset = 0;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;

		copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy.imageSubresource.mipLevel = 0;
		copy.imageSubresource.baseArrayLayer = 0;
		copy.imageSubresource.layerCount = 1;

		copy.imageOffset = { 0, 0, 0 };
		copy.imageExtent = { texture->m_Extent.width,texture->m_Extent.height,1 };

		vkCmdCopyBufferToImage(m_CommandBuffer, texture->m_StagingBuffer, texture->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = m_TransferQueueIndex;
		barrier.dstQueueFamilyIndex = m_RenderQueueIndex;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		
		vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		
	}
}

void Nexus::VulkanTransferCommandQueue::DoImageBarriers()
{
	Ref<VulkanRenderCommandQueue> queue = DynamicPointerCast<VulkanRenderCommandQueue>(Renderer::GetRenderCommandQueue());

	VkCommandBuffer cmd = queue->GetCurrentCommandBuffer();

	vkBeginCommandBuffer(cmd, &m_CommandBufferBeginInfo);

	for (auto& i : m_TransferData.m_Textures)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.image = i->m_Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = m_TransferQueueIndex;
		barrier.dstQueueFamilyIndex = m_RenderQueueIndex;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
	vkEndCommandBuffer(cmd);

	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.pCommandBuffers = &cmd;
	SubmitInfo.commandBufferCount = 1;

	vkQueueSubmit(queue->m_renderQueue, 1, &SubmitInfo, nullptr);
	vkQueueWaitIdle(queue->m_renderQueue);
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

	for(auto& i : m_Textures)
	{
		vmaDestroyBuffer(device->GetAllocator(), i->m_StagingBuffer, i->m_StagingAlloc);
		i->m_StagingBuffer = nullptr;
	}
	m_Textures.clear();
}
