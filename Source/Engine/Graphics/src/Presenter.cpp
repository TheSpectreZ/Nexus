#include "Graphics/Presenter.h"
#include "vkAssert.h"
#include "GLFW/glfw3.h"
#include "Backend.h"

Nexus::Graphics::Presenter* Nexus::Graphics::Presenter::s_Instance = nullptr;

Nexus::Graphics::EngineSpecification Nexus::Graphics::Presenter::s_Specs;
std::function<void(uint32_t,uint32_t)> Nexus::Graphics::Presenter::WindowResizeCallbackFnc = nullptr;

void Nexus::Graphics::Presenter::Init(const EngineSpecification& specs)
{
	s_Instance = new Presenter();
	s_Specs = specs;

	s_Instance->Create();

	// Fences and Semaphores
	{
		s_Instance->m_Fences.resize(s_Instance->s_FramesInFlight);
		s_Instance->m_Semaphores.first.resize(s_Instance->s_FramesInFlight);
		s_Instance->m_Semaphores.second.resize(s_Instance->s_FramesInFlight);

		VkFenceCreateInfo fInfo{};
		fInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fInfo.pNext = nullptr;
		fInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo sInfo{};
		sInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sInfo.pNext = nullptr;
		sInfo.flags = 0;

		for (uint32_t i = 0; i < s_Instance->s_FramesInFlight; i++)
		{ 
			vkCreateFence(Backend::GetDevice(), &fInfo, nullptr, &s_Instance->m_Fences[i]);
			vkCreateSemaphore(Backend::GetDevice(), &sInfo, nullptr, &s_Instance->m_Semaphores.first[i]);
			vkCreateSemaphore(Backend::GetDevice(), &sInfo, nullptr, &s_Instance->m_Semaphores.second[i]);
		}

	}

	// Command Buffers and Pool
	{
		QueueIndexFamilies Fam = Graphics::GetQueueIndexFamilies(Backend::GetPhysicalDevice(), Backend::GetSurface());

		VkCommandPoolCreateInfo pInfo{};
		pInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pInfo.pNext = nullptr;
		pInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pInfo.queueFamilyIndex = Fam.front().value();

		vkCreateCommandPool(Backend::GetDevice(), &pInfo, nullptr, &s_Instance->m_CommandPool);

		VkCommandBufferAllocateInfo aInfo{};
		aInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		aInfo.pNext = nullptr;
		aInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		aInfo.commandPool = s_Instance->m_CommandPool;
		aInfo.commandBufferCount = s_Instance->s_FramesInFlight;

		s_Instance->m_CommandBuffers.resize(s_Instance->s_FramesInFlight);
		vkAllocateCommandBuffers(Backend::GetDevice(), &aInfo, s_Instance->m_CommandBuffers.data());
	}
}

void Nexus::Graphics::Presenter::Shut()
{
	s_Instance->Destroy();
	
	for (uint32_t i = 0; i < s_Instance->s_FramesInFlight; i++)
	{
		vkDestroyFence(Backend::GetDevice(), s_Instance->m_Fences[i], nullptr);
		vkDestroySemaphore(Backend::GetDevice(), s_Instance->m_Semaphores.first[i], nullptr);
		vkDestroySemaphore(Backend::GetDevice(), s_Instance->m_Semaphores.second[i], nullptr);
	}

	vkDestroyCommandPool(Backend::GetDevice(), s_Instance->m_CommandPool, nullptr);

	NEXUS_LOG_WARN("Vulkan Presenter Shut");
	delete s_Instance;
}

void Nexus::Graphics::Presenter::StartFrame()
{
	vkWaitForFences(Backend::GetDevice(), 1, &s_Instance->m_Fences[s_Instance->s_CurrentFrame], VK_TRUE, UINT64_MAX);
	_VKR = vkAcquireNextImageKHR(Backend::GetDevice(), s_Instance->m_Swapchain, UINT64_MAX, s_Instance->m_Semaphores.first[s_Instance->s_CurrentFrame], VK_NULL_HANDLE, &s_Instance->s_ImageIndex);

	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR)
	{
		s_Instance->ReBuild();
	}

	vkResetFences(Backend::GetDevice(), 1, &s_Instance->m_Fences[s_Instance->s_CurrentFrame]);

	VkCommandBufferBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	Info.pInheritanceInfo = nullptr;
	Info.pNext = nullptr;
	Info.flags = 0;

	vkBeginCommandBuffer(s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame],&Info);
}

void Nexus::Graphics::Presenter::EndFrame()
{
	vkEndCommandBuffer(s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame]);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSubmitInfo sInfo{};
	sInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	sInfo.pNext = nullptr;
	sInfo.pCommandBuffers = &s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame];
	sInfo.commandBufferCount = 1;
	sInfo.waitSemaphoreCount = 1;
	sInfo.pWaitSemaphores = &s_Instance->m_Semaphores.first[s_Instance->s_CurrentFrame];
	sInfo.pWaitDstStageMask = waitStages;
	sInfo.signalSemaphoreCount = 1;
	sInfo.pSignalSemaphores = &s_Instance->m_Semaphores.second[s_Instance->s_CurrentFrame];

	vkQueueSubmit(Backend::GetGraphicsQueue(), 1, &sInfo, s_Instance->m_Fences[s_Instance->s_CurrentFrame]);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &s_Instance->m_Swapchain;
	presentInfo.pImageIndices = &s_Instance->s_ImageIndex;
	presentInfo.pWaitSemaphores = &s_Instance->m_Semaphores.second[s_Instance->s_CurrentFrame];
	presentInfo.waitSemaphoreCount = 1;

	_VKR = vkQueuePresentKHR(Backend::GetPresentQueue(), &presentInfo);

	if (_VKR == VK_ERROR_OUT_OF_DATE_KHR || _VKR == VK_SUBOPTIMAL_KHR)
	{
		s_Instance->ReBuild();
	}
	else if (_VKR != VK_SUCCESS)
	{
		NEXUS_ASSERT("Failed To Present Swapchain", "Swapchain Error");
	}

	s_Instance->s_CurrentFrame = (s_Instance->s_CurrentFrame + 1) % s_Instance->s_FramesInFlight;
}

void Nexus::Graphics::Presenter::BeginRenderpass(VkCommandBuffer cmdbuffer,const Renderpass& pass, const Framebuffer& buffer,std::vector<VkClearValue> values)
{
	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = pass.Get();
	Info.framebuffer = buffer.Get();
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = s_Instance->m_SwapchainExtent;
	Info.clearValueCount = (uint32_t)values.size();
	Info.pClearValues = values.data();

	vkCmdBeginRenderPass(cmdbuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::Graphics::Presenter::EndRenderpass(VkCommandBuffer buffer)
{
	vkCmdEndRenderPass(buffer);
}

void Nexus::Graphics::Presenter::SetViewportAndScissor(VkViewport* pViewport, uint32_t viewportCount, VkRect2D* pScissor, uint32_t scissorCount)
{
	vkCmdSetViewport(s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame], 0, viewportCount, pViewport);
	vkCmdSetScissor(s_Instance->m_CommandBuffers[s_Instance->s_CurrentFrame], 0, scissorCount, pScissor);
}

void Nexus::Graphics::Presenter::WaitForDevice()
{
	Backend::WaitForDevice();
}

VkSampleCountFlagBits Nexus::Graphics::Presenter::GetImageMaxSamples()
{
	return Backend::GetMaxSampleCount();
}

void Nexus::Graphics::Presenter::Create()
{
	// Swapchain
	{
		VkSwapchainCreateInfoKHR Info{};
		Info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		Info.pNext = nullptr;
		Info.surface = Backend::GetSurface();

		QueueIndexFamilies Fam = GetQueueIndexFamilies(Backend::GetPhysicalDevice(), Backend::GetSurface());
		if (Fam.front().value() != Fam.back().value())
		{
			uint32_t Indices[] = {Fam.front().value(),Fam.back().value()};
			Info.queueFamilyIndexCount = 2;
			Info.pQueueFamilyIndices = Indices;
			Info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else
		{
			Info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			Info.queueFamilyIndexCount = 0;
			Info.pQueueFamilyIndices = nullptr;
		}


		// format, colorspace, mode, capabilities, minImage
		{
			VkSurfaceCapabilitiesKHR cap;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &cap);

			uint32_t size = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, nullptr);
			std::vector<VkSurfaceFormatKHR> availableFormats(size);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, availableFormats.data());

			size = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, nullptr);
			std::vector<VkPresentModeKHR> availableModes(size);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Backend::GetPhysicalDevice(), Backend::GetSurface(), &size, availableModes.data());

			VkSurfaceFormatKHR format = availableFormats[0];
			for (auto& form : availableFormats)
			{
				if (form.format == VK_FORMAT_B8G8R8A8_UNORM && form.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					format = form;
					break;
				}
			}

			VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
			for (auto& m : availableModes)
			{
				if (m == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					mode = m;
					break;
				}
			}

			VkExtent2D extent;
			if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				extent = cap.currentExtent;
			}
			else
			{
				extent = { (uint32_t)s_Specs.targetWindow->width,(uint32_t)s_Specs.targetWindow->height };

				extent.width = std::clamp(extent.width, cap.currentExtent.width, cap.maxImageExtent.width);
				extent.height = std::clamp(extent.height, cap.currentExtent.height, cap.maxImageExtent.height);
			}

			uint32_t MinImageCount = cap.minImageCount + 1;
			if (cap.maxImageCount > 0 && MinImageCount > cap.maxImageCount)
			{
				MinImageCount = cap.maxImageCount;
			}

			Info.minImageCount = MinImageCount;
			Info.imageFormat = format.format;
			Info.imageColorSpace = format.colorSpace;
			Info.imageExtent = extent;
			Info.presentMode = mode;
			Info.preTransform = cap.currentTransform;
		}

		Info.imageArrayLayers = 1;
		Info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		Info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		Info.clipped = VK_TRUE;
		Info.oldSwapchain = VK_NULL_HANDLE;
		Info.flags = 0;

		_VKR = vkCreateSwapchainKHR(Backend::GetDevice(), &Info, nullptr, &m_Swapchain);
		CHECK_HANDLE(m_Swapchain, VkSwapchainKHR);
		NEXUS_LOG_WARN("Vulkan Swapchain Created");

		m_SwapchainExtent = Info.imageExtent;
		m_SwapchainImageFormat = Info.imageFormat;
		s_FramesInFlight = Info.minImageCount;
	}

	// Swapchain Images
	{	
		uint32_t count;
		vkGetSwapchainImagesKHR(Backend::GetDevice(), m_Swapchain, &count, nullptr);
		m_Images.clear();
		m_Images.resize(count);
		vkGetSwapchainImagesKHR(Backend::GetDevice(), m_Swapchain, &count, m_Images.data());

		NEXUS_LOG_TRACE("Vulkan Swapchain Images[{0}] Acquired", count);
	}

	// Swapchain Image Views
	{
		uint32_t count = (uint32_t)m_Images.size();

		m_ImageViews.clear();
		m_ImageViews.resize(count);

		for (uint32_t i = 0; i < count; i++)
		{
			VkImageViewCreateInfo Info{};
			Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			Info.pNext = nullptr;
			Info.flags = 0;
			Info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			Info.format = m_SwapchainImageFormat;
			Info.image = m_Images[i];
			Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Info.subresourceRange.baseArrayLayer = 0;
			Info.subresourceRange.baseMipLevel = 0;
			Info.subresourceRange.layerCount = 1;
			Info.subresourceRange.levelCount = 1;

			_VKR = vkCreateImageView(Backend::GetDevice(), &Info, nullptr, &m_ImageViews[i]);
			CHECK_HANDLE(m_ImageViews[i], VkImageView);
		}
		NEXUS_LOG_TRACE("Vulkan Swapchain Image Views[{0}] Created", count);
	}

}

void Nexus::Graphics::Presenter::Destroy()
{
	for (auto& view : m_ImageViews)
	{
		vkDestroyImageView(Backend::GetDevice(), view, nullptr);
	}

	vkDestroySwapchainKHR(Backend::GetDevice(), m_Swapchain, nullptr);
}

void Nexus::Graphics::Presenter::ReBuild()
{
	Backend::WaitForDevice();

	int width = 0, height = 0;
	glfwGetFramebufferSize(s_Specs.targetWindow->handle, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(s_Specs.targetWindow->handle, &width, &height);
		glfwWaitEvents();
	}

	NEXUS_LOG_ERROR("Presenter Recreation Started");

	s_Instance->Destroy();
	s_Instance->Create();

	if (WindowResizeCallbackFnc)
		WindowResizeCallbackFnc(m_SwapchainExtent.width,m_SwapchainExtent.height);

	NEXUS_LOG_ERROR("Presenter Recreation Completed");
}
