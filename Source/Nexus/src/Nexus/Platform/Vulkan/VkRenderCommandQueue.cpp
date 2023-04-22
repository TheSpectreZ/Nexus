#include "nxpch.h"
#include "VkRenderCommandQueue.h"

#include "VkContext.h"
#include "VkSwapchain.h"

#include "Renderer/Renderer.h"

Nexus::VulkanRenderCommandQueue::VulkanRenderCommandQueue()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	Ref<VulkanSwapchain> swapchain = VulkanSwapchain::Get();

	m_device = device->Get();
	m_swapchain = swapchain->GetHandle();
	
	m_renderQueue = device->GetGraphicsQueue();
	m_presentQueue = device->GetPresentQueue();

	m_FrameIndex = 0;
	m_ImageIndex = 0;
	m_ImageCount = swapchain->GetImageCount();

	VkCommandPoolCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	Info.queueFamilyIndex = device->GetQueueFamilyIndices().Graphics;

	_VKR = vkCreateCommandPool(m_device, &Info, nullptr, &m_CommandPool);
	CHECK_HANDLE(m_CommandPool, VkCommandPool);

	VkCommandBufferAllocateInfo aInfo{};
	aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	aInfo.pNext = nullptr;
	aInfo.commandPool = m_CommandPool;
	aInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	aInfo.commandBufferCount = swapchain->GetImageCount();

	m_CommandBuffer.resize(swapchain->GetImageCount());
	_VKR = vkAllocateCommandBuffers(m_device, &aInfo, m_CommandBuffer.data());
	CHECK_LOG_VKR;

	m_CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	m_CommandBufferBeginInfo.pNext = nullptr;
	m_CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	m_CommandBufferBeginInfo.flags = 0;

	// Sync handles
	{
		VkFenceCreateInfo fInfo{};
		fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fInfo.pNext = nullptr;
		fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo sInfo{};
		sInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sInfo.pNext = nullptr;
		sInfo.flags = 0;

		m_SyncHandles.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_ImageCount; i++)
		{
			_VKR = vkCreateFence(m_device, &fInfo, nullptr, &m_SyncHandles[i].fence);
			CHECK_LOG_VKR

			_VKR = vkCreateSemaphore(m_device, &sInfo, nullptr, &m_SyncHandles[i].imageAvailable);
			CHECK_LOG_VKR

			_VKR = vkCreateSemaphore(m_device, &sInfo, nullptr, &m_SyncHandles[i].renderFinished);
			CHECK_LOG_VKR
		}
	}

	{
		m_waitStages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_SubmitInfo.pNext = nullptr;
		m_SubmitInfo.commandBufferCount = 1;
		m_SubmitInfo.waitSemaphoreCount = 1;
		m_SubmitInfo.signalSemaphoreCount = 1;
		m_SubmitInfo.pWaitDstStageMask = m_waitStages;

		m_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		m_PresentInfo.pNext = nullptr;
		m_PresentInfo.swapchainCount = 1;
		m_PresentInfo.pSwapchains = &m_swapchain;
		m_PresentInfo.waitSemaphoreCount = 1;
	}

	NEXUS_LOG_TRACE("Vulkan Render Command Queue Created");
}

Nexus::VulkanRenderCommandQueue::~VulkanRenderCommandQueue()
{
	for (auto& s : m_SyncHandles)
	{
		vkDestroyFence(m_device, s.fence, nullptr);
		vkDestroySemaphore(m_device, s.imageAvailable, nullptr);
		vkDestroySemaphore(m_device, s.renderFinished, nullptr);
	}

	vkDestroyCommandPool(m_device, m_CommandPool, nullptr);
	NEXUS_LOG_TRACE("Vulkan Render Command Queue Destroyed");
}

void Nexus::VulkanRenderCommandQueue::Begin()
{
	vkWaitForFences(m_device, 1, &m_SyncHandles[m_FrameIndex].fence, VK_TRUE, UINT64_MAX);
	_VKR = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_SyncHandles[m_FrameIndex].imageAvailable, nullptr, &m_ImageIndex);

	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR)
	{
		CLEAR_VKR;
		VulkanSwapchain::Get()->ReCreate();
		m_swapchain = VulkanSwapchain::Get()->GetHandle();
		
		Renderer::ResizeCallback();
	}

	vkResetFences(m_device, 1, &m_SyncHandles[m_FrameIndex].fence);

	vkBeginCommandBuffer(m_CommandBuffer[m_FrameIndex], &m_CommandBufferBeginInfo);
}

void Nexus::VulkanRenderCommandQueue::End()
{
	vkEndCommandBuffer(m_CommandBuffer[m_FrameIndex]);
}

void Nexus::VulkanRenderCommandQueue::Flush()
{
	m_SubmitInfo.pCommandBuffers = &m_CommandBuffer[m_FrameIndex];
	m_SubmitInfo.pWaitSemaphores = &m_SyncHandles[m_FrameIndex].imageAvailable;
	m_SubmitInfo.pSignalSemaphores = &m_SyncHandles[m_FrameIndex].renderFinished;

	vkQueueSubmit(m_renderQueue, 1, &m_SubmitInfo, m_SyncHandles[m_FrameIndex].fence);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.pImageIndices = &m_ImageIndex;
	presentInfo.pWaitSemaphores = &m_SyncHandles[m_FrameIndex].renderFinished;
	presentInfo.waitSemaphoreCount = 1;

	_VKR = vkQueuePresentKHR(m_presentQueue, &presentInfo);

	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR || _VKR == VK_SUBOPTIMAL_KHR)
	{
		CLEAR_VKR;
		VulkanSwapchain::Get()->ReCreate();
		m_swapchain = VulkanSwapchain::Get()->GetHandle();
		
		Renderer::ResizeCallback();
	}
	else if (_VKR != VK_SUCCESS)
	{
		NEXUS_ASSERT("Failed To Present Swapchain", "Swapchain Error");
	}

	m_FrameIndex = (m_FrameIndex + 1) % m_ImageCount;
}

void Nexus::VulkanRenderCommandQueue::Render()
{
}
