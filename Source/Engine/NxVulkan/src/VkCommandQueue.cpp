#include "NxVulkan/VkCommandQueue.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkRenderPass.h"
#include "NxVulkan/VkFramebuffer.h"
#include "NxVulkan/VkPipeline.h"
#include "NxVulkan/VkShader.h"

void Nexus::VulkanCommandQueue::Transferdata::Clear()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	for (auto& b : m_Buffer)
	{
		vmaDestroyBuffer(device->GetAllocator(), b->m_stagBuf, b->m_stagAlloc);
		b->m_stagBuf = nullptr;
	}
	m_Buffer.clear();

	for (auto& b : m_StaticBuffer)
	{
		vmaDestroyBuffer(device->GetAllocator(), b->m_StagingBuff, b->m_StagingAlloc);
		b->m_StagingBuff = nullptr;
	}
	m_StaticBuffer.clear();

	for (auto& i : m_Textures)
	{
		vmaDestroyBuffer(device->GetAllocator(), i->m_StagingBuffer, i->m_StagingAlloc);
		i->m_StagingBuffer = nullptr;
	}
	m_Textures.clear();
}

Nexus::VulkanCommandQueue* Nexus::VulkanCommandQueue::s_Instance = nullptr;

Nexus::VulkanCommandQueue::VulkanCommandQueue(std::function<void()> resizeCallback)
{
	m_ResizeCallback = resizeCallback;
}

void Nexus::VulkanCommandQueue::Initialize()
{
	s_Instance = this;
	
	m_Device = VulkanContext::Get()->GetDeviceRef();

	m_RenderQueue = m_Device->GetGraphicsQueue();
	m_PresentQueue = m_Device->GetPresentQueue();
	m_TransferQueue = m_Device->GetTransferQueue();

	// Command Pools
	{
		VkCommandPoolCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		Info.queueFamilyIndex = m_Device->GetQueueFamilyIndices().Graphics;

		_VKR = vkCreateCommandPool(m_Device->Get(), &Info, nullptr, &m_RenderCommandPool);
		CHECK_HANDLE(m_RenderCommandPool, VkCommandPool);

		Info.queueFamilyIndex = m_Device->GetQueueFamilyIndices().Transfer;

		_VKR = vkCreateCommandPool(m_Device->Get(), &Info, nullptr, &m_TransferCommandPool);
		CHECK_HANDLE(m_TransferCommandPool, VkCommandPool);
	}

	// Command Buffers
	{
		VkCommandBufferAllocateInfo aInfo{};
		aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		aInfo.pNext = nullptr;
		aInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		aInfo.commandBufferCount = VulkanSwapchain::Get()->GetImageCount();

		aInfo.commandPool = m_RenderCommandPool;
		m_RenderCommandBuffer.resize(aInfo.commandBufferCount);
		_VKR = vkAllocateCommandBuffers(m_Device->Get(), &aInfo, m_RenderCommandBuffer.data());
		CHECK_LOG_VKR;;
		
		aInfo.commandPool = m_TransferCommandPool;
		m_TransferCommandBuffer.resize(aInfo.commandBufferCount);
		_VKR = vkAllocateCommandBuffers(m_Device->Get(), &aInfo, m_TransferCommandBuffer.data());
		CHECK_LOG_VKR;
	}

	// Syncronization
	{
		VkFenceCreateInfo fInfo{};
		fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fInfo.pNext = nullptr;
		fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo sInfo{};
		sInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sInfo.pNext = nullptr;
		sInfo.flags = 0;

		for (uint32_t i = 0; i < VulkanSwapchain::Get()->GetImageCount(); i++)
		{
			_VKR = vkCreateFence(m_Device->Get(), &fInfo, nullptr, &m_RenderFences.emplace_back());
			CHECK_LOG_VKR;

			_VKR = vkCreateSemaphore(m_Device->Get(), &sInfo, nullptr, &m_ImageAvailableSemaphore.emplace_back());
			CHECK_LOG_VKR;

			_VKR = vkCreateSemaphore(m_Device->Get(), &sInfo, nullptr, &m_RenderFinishedSemaphore.emplace_back());
			CHECK_LOG_VKR;
		}
	}

	// Info Structs
	{
		m_CmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		m_CmdBeginInfo.pNext = nullptr;
		m_CmdBeginInfo.pInheritanceInfo = nullptr;
		m_CmdBeginInfo.flags = 0;

		m_RenderWaitStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_RenderSubmitInfo = {};
		m_RenderSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_RenderSubmitInfo.pNext = nullptr;
		m_RenderSubmitInfo.commandBufferCount = 1;
		m_RenderSubmitInfo.signalSemaphoreCount = 1;
		m_RenderSubmitInfo.waitSemaphoreCount = 1;
		m_RenderSubmitInfo.pWaitDstStageMask = &m_RenderWaitStageFlag;

		m_TransferSubmitInfo = {};
		m_TransferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_TransferSubmitInfo.pNext = nullptr;
		m_TransferSubmitInfo.commandBufferCount = 1;

		m_PresentInfo = {};
		m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		m_PresentInfo.pNext = nullptr;
		m_PresentInfo.swapchainCount = 1;
		m_PresentInfo.waitSemaphoreCount = 1;
	}

	m_FrameIndex = 0;
	m_ImageCount = VulkanSwapchain::Get()->GetImageCount();
	m_Swapchain = VulkanSwapchain::Get()->GetHandle();

	m_RenderQueueIndex = m_Device->GetQueueFamilyIndices().Graphics;
	m_TransferQueueIndex = m_Device->GetQueueFamilyIndices().Transfer;

	NEXUS_LOG("Vulkan", "Command Queue Created");
}

void Nexus::VulkanCommandQueue::Shutdown()
{
	for (size_t i = 0; i < m_RenderFences.size(); i++)
	{
		vkDestroyFence(m_Device->Get(), m_RenderFences[i], nullptr);
		vkDestroySemaphore(m_Device->Get(), m_ImageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(m_Device->Get(), m_RenderFinishedSemaphore[i], nullptr);
	}

	vkDestroyCommandPool(m_Device->Get(), m_RenderCommandPool, nullptr);
	vkDestroyCommandPool(m_Device->Get(), m_TransferCommandPool, nullptr);

	NEXUS_LOG("Vulkan", "Command Queue Destroyed");
}

void Nexus::VulkanCommandQueue::BeginRenderQueue()
{
	vkWaitForFences(m_Device->Get(), 1, &m_RenderFences[m_FrameIndex], VK_TRUE, UINT64_MAX);
	_VKR = vkAcquireNextImageKHR(m_Device->Get(), m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore[m_FrameIndex], nullptr, &m_ImageIndex);

	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR)
	{
		CLEAR_VKR;
		VulkanSwapchain::Get()->ReCreate();
		m_Swapchain = VulkanSwapchain::Get()->GetHandle();

		m_ResizeCallback();
	}

	vkResetFences(m_Device->Get(), 1, &m_RenderFences[m_FrameIndex]);
	
	vkBeginCommandBuffer(m_RenderCommandBuffer[m_FrameIndex], &m_CmdBeginInfo);
}

void Nexus::VulkanCommandQueue::EndRenderQueue()
{
	vkEndCommandBuffer(m_RenderCommandBuffer[m_FrameIndex]);
}

void Nexus::VulkanCommandQueue::FlushRenderQueue()
{
	m_RenderSubmitInfo.pCommandBuffers = &m_RenderCommandBuffer[m_FrameIndex];
	m_RenderSubmitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore[m_FrameIndex];
	m_RenderSubmitInfo.pSignalSemaphores = &m_RenderFinishedSemaphore[m_FrameIndex];

	_VKR = vkQueueSubmit(m_RenderQueue, 1, &m_RenderSubmitInfo, m_RenderFences[m_FrameIndex]);
	CHECK_LOG_VKR;

	m_PresentInfo.pSwapchains = &m_Swapchain;
	m_PresentInfo.pImageIndices = &m_ImageIndex;
	m_PresentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore[m_FrameIndex];
	m_PresentInfo.waitSemaphoreCount = 1;
	
	_VKR = vkQueuePresentKHR(m_PresentQueue, &m_PresentInfo);
	CHECK_LOG_VKR;
	
	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR || _VKR == VK_SUBOPTIMAL_KHR)
	{
		CLEAR_VKR;
		VulkanSwapchain::Get()->ReCreate();
		m_Swapchain = VulkanSwapchain::Get()->GetHandle();
	
		m_ResizeCallback();
	}
	else if (_VKR != VK_SUCCESS)
	{
		NEXUS_ASSERT("Failed To Present Swapchain", "Swapchain Error");
	}

	m_FrameIndex = (m_FrameIndex + 1) % m_ImageCount;
}

void Nexus::VulkanCommandQueue::FlushTransferQueue()
{
	//NEXUS_SCOPED_PROFILE("Entire Transfer Queue");

	if (m_TransferData.Empty())
		return;

	vkBeginCommandBuffer(m_TransferCommandBuffer[m_FrameIndex], &m_CmdBeginInfo);
	{
		for (auto& buffer : m_TransferData.m_Buffer)
		{
			VkBufferCopy copy{};
			copy.size = buffer->m_Size;
			copy.dstOffset = 0;
			copy.srcOffset = 0;

			vkCmdCopyBuffer(m_TransferCommandBuffer[m_FrameIndex], buffer->m_stagBuf, buffer->m_Buffer, 1, &copy);
			NEXUS_LOG("Vulkan Debug", "Copied Buffer");
		}

		// Static Buffer
		for (auto& buffer : m_TransferData.m_StaticBuffer)
		{
			VkBufferCopy copy{};
			copy.size = buffer->m_size;
			copy.dstOffset = 0;
			copy.srcOffset = 0;

			vkCmdCopyBuffer(m_TransferCommandBuffer[m_FrameIndex], buffer->m_StagingBuff, buffer->m_buffer, 1, &copy);
		}

		// Texture
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
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(m_TransferCommandBuffer[m_FrameIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

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

			vkCmdCopyBufferToImage(m_TransferCommandBuffer[m_FrameIndex], texture->m_StagingBuffer, texture->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = m_TransferQueueIndex;
			barrier.dstQueueFamilyIndex = m_RenderQueueIndex;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = 0;

			vkCmdPipelineBarrier(m_TransferCommandBuffer[m_FrameIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		}
	}
	vkEndCommandBuffer(m_TransferCommandBuffer[m_FrameIndex]);

	m_TransferSubmitInfo.pCommandBuffers = &m_TransferCommandBuffer[m_FrameIndex];
	_VKR = vkQueueSubmit(m_TransferQueue, 1, &m_TransferSubmitInfo, nullptr);
	vkQueueWaitIdle(m_TransferQueue);

	vkBeginCommandBuffer(m_RenderCommandBuffer[m_FrameIndex], &m_CmdBeginInfo);
	{
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
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(m_RenderCommandBuffer[m_FrameIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		}
	}
	vkEndCommandBuffer(m_RenderCommandBuffer[m_FrameIndex]);

	m_TransferSubmitInfo.pCommandBuffers = &m_RenderCommandBuffer[m_FrameIndex];
	_VKR = vkQueueSubmit(m_RenderQueue, 1, &m_TransferSubmitInfo, nullptr);
	vkQueueWaitIdle(m_RenderQueue);

	m_TransferData.Clear();
}

void Nexus::VulkanCommandQueue::BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer)
{
	Ref<VulkanRenderpass> renderpass = DynamicPointerCast<VulkanRenderpass>(pass);
	Ref<VulkanFramebuffer> frame = DynamicPointerCast<VulkanFramebuffer>(framebuffer);

	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = renderpass->Get();
	Info.framebuffer = frame->Get(m_FrameIndex);
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = frame->GetExtent();
	Info.clearValueCount = (uint32_t)frame->GetClearValues().size();
	Info.pClearValues = frame->GetClearValues().data();

	vkCmdBeginRenderPass(m_RenderCommandBuffer[m_FrameIndex], &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::VulkanCommandQueue::EndRenderPass()
{
	vkCmdEndRenderPass(m_RenderCommandBuffer[m_FrameIndex]);
}

void Nexus::VulkanCommandQueue::BindShaderResourceHeap(Ref<Shader> shader, ResourceHeapHandle handle)
{
	Ref<VulkanShader> vks = DynamicPointerCast<VulkanShader>(shader);

	VkDescriptorSet& Set = vks->m_SetResource[handle.set].Heaps[handle.hashId].Get();
	vkCmdBindDescriptorSets(m_RenderCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vks->m_Layout, handle.set, 1, &Set, 0, nullptr);
}

void Nexus::VulkanCommandQueue::BindPipeline(Ref<Pipeline> pipeline)
{
	Ref<VulkanPipeline> Vkp = DynamicPointerCast<VulkanPipeline>(pipeline);
	vkCmdBindPipeline(m_RenderCommandBuffer[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Vkp->Get());
}

void Nexus::VulkanCommandQueue::SetScissor(Scissor scissor)
{
	m_Scissor.offset = { scissor.Offset.x, scissor.Offset.y };
	m_Scissor.extent = { scissor.Extent.width,scissor.Extent.height };

	vkCmdSetScissor(m_RenderCommandBuffer[m_FrameIndex], 0, 1, &m_Scissor);
}

void Nexus::VulkanCommandQueue::SetViewport(Viewport viewport)
{
	m_Viewport.x = viewport.x;
	m_Viewport.y = viewport.y;
	m_Viewport.width = viewport.width;
	m_Viewport.height = viewport.height;
	m_Viewport.minDepth = viewport.minDepth;
	m_Viewport.maxDepth = viewport.maxDepth;

	vkCmdSetViewport(m_RenderCommandBuffer[m_FrameIndex], 0, 1, &m_Viewport);
}

void Nexus::VulkanCommandQueue::TransferBufferToGPU(VulkanBuffer* buffer)
{
	m_TransferData.m_Buffer.push_back(buffer);
}

void Nexus::VulkanCommandQueue::TransferBufferToGPU(VulkanStaticBuffer* buf)
{
	m_TransferData.m_StaticBuffer.push_back(buf);
}

void Nexus::VulkanCommandQueue::TransferTextureToGPU(VulkanTexture* texture)
{
	m_TransferData.m_Textures.push_back(texture);
}

void Nexus::VulkanCommandQueue::BindVertexBuffer(Ref<Buffer> buffer)
{
	Ref<VulkanBuffer> buf = DynamicPointerCast<VulkanBuffer>(buffer);

	VkBuffer vkbuf[] = { buf->Get() };
	VkDeviceSize off[] = { 0 };

	vkCmdBindVertexBuffers(m_RenderCommandBuffer[m_FrameIndex], 0, 1, vkbuf, off);

}

void Nexus::VulkanCommandQueue::BindIndexBuffer(Ref<Buffer> buffer)
{
	Ref<VulkanBuffer> buf = DynamicPointerCast<VulkanBuffer>(buffer);
	vkCmdBindIndexBuffer(m_RenderCommandBuffer[m_FrameIndex], buf->Get(), 0, VK_INDEX_TYPE_UINT32);
}

void Nexus::VulkanCommandQueue::DrawIndices(uint32_t IndexCount)
{
	vkCmdDrawIndexed(m_RenderCommandBuffer[m_FrameIndex], IndexCount, 1, 0, 0, 0);
}

//void Nexus::VulkanCommandQueue::DrawSubMesh(SubMesh* submesh)
//{
//	Ref<VulkanStaticBuffer> vb = DynamicPointerCast<VulkanStaticBuffer>(submesh->vb);
//	Ref<VulkanStaticBuffer> ib = DynamicPointerCast<VulkanStaticBuffer>(submesh->ib);
//
//	VkBuffer buf[] = { vb->Get() };
//	VkDeviceSize off[] = { 0 };
//
//	vkCmdBindVertexBuffers(m_RenderCommandBuffer[m_FrameIndex], 0, 1, buf, off);
//	vkCmdBindIndexBuffer(m_RenderCommandBuffer[m_FrameIndex], ib->Get(), 0, VK_INDEX_TYPE_UINT32);
//	vkCmdDrawIndexed(m_RenderCommandBuffer[m_FrameIndex], ib->m_size / sizeof(uint32_t), 1, 0, 0, 0);
//}
//
//void Nexus::VulkanCommandQueue::DrawMesh(Ref<StaticMesh> mesh)
//{
//	auto& submeshes = mesh->GetSubMeshes();
//	for (auto& sm : submeshes)
//	{
//		if (!sm.draw)
//			continue;
//	
//		DrawSubMesh(&sm);
//	}
//}