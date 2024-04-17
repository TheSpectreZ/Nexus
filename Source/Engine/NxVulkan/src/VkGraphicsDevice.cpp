#define NOMINMAX
#define VMA_IMPLEMENTATION
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#include "NxVulkan/VkGraphicsDevice.h"

#include "NxCore/Base.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxVulkan/VkSpecification.h"

static bool CheckLayersAvailability(std::vector<const char*> Layers)
{
	auto AvailableLayers = vk::enumerateInstanceLayerProperties();

	for (auto& layer : Layers)
	{
		bool found = false;
		for (auto& al : AvailableLayers)
		{
			if (strcmp(layer, al.layerName.data()) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
			return false;
	}

	return true;
}

static bool CheckExtensionAvailability(std::vector<const char*> extensions, vk::PhysicalDevice device)
{
	auto availableExtensions = device.enumerateDeviceExtensionProperties();

	for (auto& ext : extensions)
	{
		bool found = false;
		for (auto& e : availableExtensions)
		{
			if (strcmp(ext, e.extensionName.data()) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
			return false;
	}

	return true;
}

static VkBool32 DebugUtilsMessengerCallbackEXT(
	VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	printf("\n");
	NEXUS_LOG("Vulkan", "%s", pCallbackData->pMessage);
	printf("\n");
	return VK_FALSE;
}

bool Nexus::VulkanGraphicsDevice::Initialize(const GraphicsDeviceSpecifications& specs)
{
	mWindow = specs.pWindow;

	std::vector<const char*> RequiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };
	std::vector<const char*> RequiredLayers =
	{
		#ifdef NEXUS_DEBUG
			"VK_LAYER_KHRONOS_validation"
		#endif;
	};

	bool debugEnabled = false;

#ifdef NEXUS_DEBUG
	debugEnabled = CheckLayersAvailability(RequiredLayers);
	if (!debugEnabled)
		RequiredLayers.pop_back();
#endif // NEXUS_DEBUG

	// Instance
	{
		vk::ApplicationInfo app{};
		app.apiVersion = vk::ApiVersion13;

		vk::InstanceCreateInfo info{};
		info.pApplicationInfo = &app;

		std::vector<const char*> InstanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		if (debugEnabled)
			InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		info.setPEnabledExtensionNames(InstanceExtensions);
		info.setPEnabledLayerNames(RequiredLayers);

		mInstance = vk::createInstance(info);

	}

	// Debug Messenger
	{
#ifdef NEXUS_DEBUG

		vk::DebugUtilsMessengerCreateInfoEXT info{};
		info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
		info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
		info.pfnUserCallback = DebugUtilsMessengerCallbackEXT;

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");
		
		VkDebugUtilsMessengerEXT messenger;
		if (func)
		{
			auto vkr = func(mInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&info), nullptr, &messenger);
			mDebugMessenger = vkr == VK_SUCCESS ? messenger : nullptr;
		}
#endif // NEXUS_DEBUG
	}

	// Physical Device
	{
		auto devices = mInstance.enumeratePhysicalDevices();
		mPhysicalDevice = devices[0];

		for (auto& device : devices)
		{
			auto props = device.getProperties();

			if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				if (CheckExtensionAvailability(RequiredExtensions, device))
				{
					mPhysicalDevice = device;
					break;
				}

			}

		}

		mPhysicalDeviceProps = mPhysicalDevice.getProperties();
		NEXUS_LOG("Vulkan", "Physical Device Acquired: %s", mPhysicalDeviceProps.deviceName);

		// Depth Format
		{
			std::vector<vk::Format> candidates = {
				vk::Format::eD32SfloatS8Uint,
				vk::Format::eD32Sfloat,
				vk::Format::eD24UnormS8Uint,
				vk::Format::eD16UnormS8Uint,
				vk::Format::eD16Unorm
			};

			mDepthFormat = vk::Format::eUndefined;
			for (auto& format : candidates)
			{
				auto props = mPhysicalDevice.getFormatProperties(format);
				if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
					mDepthFormat = format;
			}

			NEXUS_ASSERT(mDepthFormat == vk::Format::eUndefined, "No Suitable Depth Format Available");
		}

		// Max Sample Count
		{
			auto counts = mPhysicalDeviceProps.limits.framebufferColorSampleCounts & mPhysicalDeviceProps.limits.framebufferDepthSampleCounts;

			std::array<vk::SampleCountFlagBits, 6> candidates =
			{
				vk::SampleCountFlagBits::e2,
				vk::SampleCountFlagBits::e4,
				vk::SampleCountFlagBits::e8,
				vk::SampleCountFlagBits::e16,
				vk::SampleCountFlagBits::e32,
				vk::SampleCountFlagBits::e64
			};

			mMaxSampleCount = vk::SampleCountFlagBits::e1;
			for (auto& e : candidates)
			{
				if (counts & e)
				{
					mMaxSampleCount = e;
					break;
				}
			}
		}
	}

	// Device
	{
		auto flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute;
		mQueueFamIndices = QueryQueueFamilyIndices(flags);

		float queuePriority = 0.f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

		if (IsFlagSet(flags, vk::QueueFlagBits::eGraphics))
		{
			NEXUS_ASSERT((mQueueFamIndices.Graphics == -1), "No Graphics Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = mQueueFamIndices.Graphics;
		}

		if (IsFlagSet(flags, vk::QueueFlagBits::eCompute))
		{
			NEXUS_ASSERT((mQueueFamIndices.Compute == -1), "No Compute Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = mQueueFamIndices.Compute;
		}

		if (IsFlagSet(flags, vk::QueueFlagBits::eTransfer))
		{
			NEXUS_ASSERT((mQueueFamIndices.Transfer == -1), "No Transfer Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = mQueueFamIndices.Transfer;
		}

		vk::PhysicalDeviceFeatures features{};
		features.samplerAnisotropy = vk::True;
		features.sampleRateShading = vk::True;
		features.fillModeNonSolid = vk::True;

		vk::PhysicalDeviceDynamicRenderingFeatures dynamicRendering{};
		dynamicRendering.dynamicRendering = vk::True;
		
		vk::DeviceCreateInfo info{};
		info.setPNext(&dynamicRendering);
		info.setPEnabledFeatures(&features);
		info.setQueueCreateInfos(queueCreateInfo);
		info.setPEnabledExtensionNames(RequiredExtensions);
		info.setPEnabledLayerNames(RequiredLayers);

		mDevice = mPhysicalDevice.createDevice(info);

		mGraphicsQueue = mDevice.getQueue(mQueueFamIndices.Graphics, 0);
		mTransferQueue = mDevice.getQueue(mQueueFamIndices.Transfer, 0);
		mComputeQueue = mDevice.getQueue(mQueueFamIndices.Compute, 0);
	}

	// Vulkan Memory ALlocator
	{
		VmaAllocatorCreateInfo Info{};
		Info.instance = mInstance;
		Info.physicalDevice = mPhysicalDevice;
		Info.device = mDevice;

		vkEnumerateInstanceVersion(&Info.vulkanApiVersion);

		vmaCreateAllocator(&Info, &mAllocator);
	}

	// Surface
	{
		vk::Win32SurfaceCreateInfoKHR info{};
		info.hwnd = (HWND)specs.pWindow->nativeHandle;
		info.hinstance = (HINSTANCE)specs.hInstance;

		mSurface = mInstance.createWin32SurfaceKHR(info);
	}

	// Screen Resources
	{
		mSwapchainDetails = QuerySwapchainDetails();
		CreateScreenResources(specs.pWindow->width, specs.pWindow->height);
	}

	// Command Pool
	{
		vk::CommandPoolCreateInfo gInfo{};
		gInfo.queueFamilyIndex = mQueueFamIndices.Graphics;
		gInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		mGraphicsCommandPool = mDevice.createCommandPool(gInfo);

		vk::CommandBufferAllocateInfo gcInfo;
		gcInfo.commandPool = mGraphicsCommandPool;
		gcInfo.level = vk::CommandBufferLevel::ePrimary;
		gcInfo.commandBufferCount = mSwapchainDetails.minImageCount;

		mGraphicsCommandBuffer = mDevice.allocateCommandBuffers(gcInfo);
	}

	// Fences & Semaphores
	{
		vk::FenceCreateInfo fInfo{};
		fInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		mFence.resize(mSwapchainDetails.minImageCount);
		mImageAvailSemaphore.resize(mSwapchainDetails.minImageCount);
		mRenderFinishedSemaphore.resize(mSwapchainDetails.minImageCount);

		for (uint32_t i = 0; i < mSwapchainDetails.minImageCount; i++)
		{
			mFence[i] = mDevice.createFence(fInfo);
			mImageAvailSemaphore[i] = mDevice.createSemaphore(vk::SemaphoreCreateInfo{});
			mRenderFinishedSemaphore[i] = mDevice.createSemaphore(vk::SemaphoreCreateInfo{});
		}
	}

	return true;
}

void Nexus::VulkanGraphicsDevice::Shutdown()
{
	DestroyScreenResources();
	mInstance.destroySurfaceKHR(mSurface);
	
	for (uint32_t i = 0; i < mSwapchainDetails.minImageCount; i++)
	{
		mDevice.destroyFence(mFence[i]);
		mDevice.destroySemaphore(mImageAvailSemaphore[i]);
		mDevice.destroySemaphore(mRenderFinishedSemaphore[i]);
	}

	mDevice.destroyCommandPool(mGraphicsCommandPool);
	
	vmaDestroyAllocator(mAllocator);
		
	mDevice.destroy();

	// Debug Messenger
	{
#ifdef NEXUS_DEBUG

		if (mDebugMessenger)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");
			if (func) func(mInstance, mDebugMessenger, nullptr);
		}

#endif // NEXUS_DEBUG
	}

	mInstance.destroy();
}

void Nexus::VulkanGraphicsDevice::WaitForGPU()
{
	mDevice.waitIdle();
}

void Nexus::VulkanGraphicsDevice::BeginFrame()
{
	static auto num_max = std::numeric_limits<uint64_t>::max();

	mDevice.waitForFences(mFence[mFrameIndex], vk::True, num_max);

	auto vkr = mDevice.acquireNextImageKHR(mSwapchain, num_max, mImageAvailSemaphore[mFrameIndex], nullptr, &mImageIndex);
	if (vkr == vk::Result::eErrorOutOfDateKHR)
	{
		OnScreenResize(mWindow->width, mWindow->height);
	}
	
	mDevice.resetFences(mFence[mFrameIndex]);
	mGraphicsCommandBuffer[mFrameIndex].begin(vk::CommandBufferBeginInfo{});
}

void Nexus::VulkanGraphicsDevice::EndFrame(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex)
{
	auto& cmd = mGraphicsCommandBuffer[mFrameIndex];

	mColorAttachmentInfo.imageView = mImages[mImageIndex].view;

	// [layout change] undefined ---> TransferDst
	{
		vk::ImageMemoryBarrier imageBarrier;
		imageBarrier.oldLayout = vk::ImageLayout::eUndefined;
		imageBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
		imageBarrier.image = mImages[mImageIndex].image;
		imageBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
		imageBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		imageBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.layerCount = 1;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = 1;

		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, 
			{}, {}, imageBarrier);
	}
	
	// Write to Swapchain Image
	{
		Ref<VulkanFramebuffer> fb = DynamicPointerCast<VulkanFramebuffer>(framebuffer);
		
		auto srcExtent = fb->getExtent();
		auto srcAttachment = fb->getAttachment(mFrameIndex, attachmentIndex);

		vk::ImageMemoryBarrier imageBarrier;
		imageBarrier.image = srcAttachment.image;
		imageBarrier.subresourceRange = srcAttachment.subresource;;
		imageBarrier.oldLayout = srcAttachment.layout;
		imageBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		imageBarrier.srcAccessMask = GetAccessFlagFromLayout(srcAttachment.layout);
		imageBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
		
		auto stageFlag = GetPipelineStageFromLayout(srcAttachment.layout);
		cmd.pipelineBarrier(stageFlag, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, {}, {}, imageBarrier);
		
		vk::ImageBlit blit;
		blit.srcOffsets[0] = vk::Offset3D(0, 0, 0);
		blit.srcOffsets[1] = vk::Offset3D(srcExtent.width, srcExtent.height, 1);
		blit.dstOffsets[0] = vk::Offset3D(0, 0, 0);
		blit.dstOffsets[1] = vk::Offset3D(mSwapchainDetails.extent.width, mSwapchainDetails.extent.height, 1);
		blit.srcSubresource.aspectMask = srcAttachment.subresource.aspectMask;
		blit.srcSubresource.mipLevel = srcAttachment.subresource.baseMipLevel;
		blit.srcSubresource.layerCount = srcAttachment.subresource.layerCount;
		blit.srcSubresource.baseArrayLayer = srcAttachment.subresource.baseArrayLayer;
		blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.dstSubresource.mipLevel = 0;
		blit.dstSubresource.layerCount = 1;
		blit.dstSubresource.baseArrayLayer = 0;

		auto& dstImage = mImages.at(mFrameIndex);
		cmd.blitImage(srcAttachment.image, vk::ImageLayout::eTransferSrcOptimal, dstImage.image, vk::ImageLayout::eTransferDstOptimal, blit, vk::Filter::eLinear);
	}

	// [layout change] TransferDst ---> presentSrc
	{
		vk::ImageMemoryBarrier imageBarrier;
		imageBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		imageBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
		imageBarrier.image = mImages[mImageIndex].image;
		imageBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		imageBarrier.dstAccessMask = vk::AccessFlagBits::eNone;
		imageBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.layerCount = 1;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = 1;

		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags{},
			{}, {}, imageBarrier);
	}

	cmd.end();
}

void Nexus::VulkanGraphicsDevice::Flush()
{
	static vk::PipelineStageFlags waitFlags[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::SubmitInfo sInfo{};
	sInfo.setCommandBuffers(mGraphicsCommandBuffer[mFrameIndex]);
	sInfo.setWaitSemaphores(mImageAvailSemaphore[mFrameIndex]);
	sInfo.setSignalSemaphores(mRenderFinishedSemaphore[mFrameIndex]);
	sInfo.setWaitDstStageMask(waitFlags);

	mGraphicsQueue.submit(sInfo, mFence[mFrameIndex]);

	vk::PresentInfoKHR pInfo{};
	pInfo.setSwapchains(mSwapchain);
	pInfo.setWaitSemaphores(mRenderFinishedSemaphore[mFrameIndex]);
	pInfo.pImageIndices = &mImageIndex;

	auto vkr = mGraphicsQueue.presentKHR(pInfo);

	if (vkr == vk::Result::eErrorOutOfDateKHR || vkr == vk::Result::eSuboptimalKHR)
	{
		OnScreenResize(mWindow->width, mWindow->height);
	}

	mFrameIndex = (mFrameIndex + 1) % mSwapchainDetails.minImageCount;
}

void Nexus::VulkanGraphicsDevice::OnScreenResize(int width, int height)
{
	DestroyScreenResources();
	CreateScreenResources(width, height);
}

Nexus::VulkanGraphicsDevice::QueueFamilyIndices Nexus::VulkanGraphicsDevice::QueryQueueFamilyIndices(vk::QueueFlags flags)
{
	QueueFamilyIndices indices;
	auto QueueFamilyProps = mPhysicalDevice.getQueueFamilyProperties();

	// Dedicated queue for compute
	if (IsFlagSet(flags, vk::QueueFlagBits::eCompute))
	{
		for (uint32_t i = 0; i < QueueFamilyProps.size(); i++)
		{
			auto& props = QueueFamilyProps[i];
			if (IsFlagSet(props.queueFlags, vk::QueueFlagBits::eCompute) && !IsFlagSet(props.queueFlags, vk::QueueFlagBits::eGraphics))
			{
				indices.Compute = i;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	if (IsFlagSet(flags, vk::QueueFlagBits::eTransfer))
	{
		for (uint32_t i = 0; i < QueueFamilyProps.size(); i++)
		{
			auto& props = QueueFamilyProps[i];
			if (IsFlagSet(props.queueFlags, vk::QueueFlagBits::eTransfer) && !IsFlagSet(props.queueFlags, vk::QueueFlagBits::eGraphics) && !IsFlagSet(props.queueFlags, vk::QueueFlagBits::eCompute))
			{
				indices.Transfer = i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < QueueFamilyProps.size(); i++)
	{
		auto& props = QueueFamilyProps[i];

		if ( IsFlagSet(flags, vk::QueueFlagBits::eTransfer) && indices.Transfer == -1)
		{
			if (IsFlagSet(props.queueFlags, vk::QueueFlagBits::eTransfer))
				indices.Transfer = i;
		}

		if (IsFlagSet(flags, vk::QueueFlagBits::eCompute) && indices.Compute == -1)
		{
			if (IsFlagSet(props.queueFlags, vk::QueueFlagBits::eCompute))
				indices.Compute = i;
		}
		
		if (IsFlagSet(flags, vk::QueueFlagBits::eGraphics) && indices.Graphics == -1)
		{
			if (IsFlagSet(props.queueFlags, vk::QueueFlagBits::eGraphics))
				indices.Graphics = i;
		}
	}

	return indices;
}

Nexus::VulkanGraphicsDevice::SwapchainDetails Nexus::VulkanGraphicsDevice::QuerySwapchainDetails()
{
	SwapchainDetails details;

	auto availableFormats = mPhysicalDevice.getSurfaceFormatsKHR(mSurface);
	auto availableModes = mPhysicalDevice.getSurfacePresentModesKHR(mSurface);
	
	details.surfaceFormat = availableFormats[0];
	for (auto& form : availableFormats)
	{
		if (form.format == vk::Format::eB8G8R8A8Unorm && form.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			details.surfaceFormat = form;
			break;
		}
	}

	details.presentMode = vk::PresentModeKHR::eFifo;
	for (auto& m : availableModes)
	{
		if (m == vk::PresentModeKHR::eMailbox)
		{
			details.presentMode = m;
			break;
		}
	}

	auto cap = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

	details.minImageCount = cap.minImageCount + 1;
	if (cap.maxImageCount > 0 && 
		details.minImageCount > cap.maxImageCount)
	{
		details.minImageCount = cap.maxImageCount;
	}

	return details;
}

void Nexus::VulkanGraphicsDevice::CreateScreenResources(int width, int height)
{
	auto cap = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

	if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		mSwapchainDetails.extent = cap.currentExtent;
	}
	else
	{
		mSwapchainDetails.extent = { (uint32_t)width,(uint32_t)height };

		mSwapchainDetails.extent.width = std::clamp(mSwapchainDetails.extent.width, cap.currentExtent.width, cap.maxImageExtent.width);
		mSwapchainDetails.extent.height = std::clamp(mSwapchainDetails.extent.height, cap.currentExtent.height, cap.maxImageExtent.height);
	}
	
	vk::SwapchainCreateInfoKHR info{};
	info.surface = mSurface;
	info.minImageCount = mSwapchainDetails.minImageCount;
	info.imageFormat = mSwapchainDetails.surfaceFormat.format;
	info.imageColorSpace = mSwapchainDetails.surfaceFormat.colorSpace;
	info.imageExtent = mSwapchainDetails.extent;
	info.presentMode = mSwapchainDetails.presentMode;
	info.preTransform = cap.currentTransform;
	info.imageArrayLayers = 1;
	info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	info.clipped = vk::True;
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

	if (IsFlagSet(cap.supportedUsageFlags, vk::ImageUsageFlagBits::eTransferSrc))
		info.imageUsage |= vk::ImageUsageFlagBits::eTransferSrc;

	if (IsFlagSet(cap.supportedUsageFlags, vk::ImageUsageFlagBits::eTransferDst))
		info.imageUsage |= vk::ImageUsageFlagBits::eTransferDst;

	mSwapchain = mDevice.createSwapchainKHR(info);
	
	auto images = mDevice.getSwapchainImagesKHR(mSwapchain);
	for (auto& image : images)
	{
		vk::ImageViewCreateInfo vInfo{};
		vInfo.viewType = vk::ImageViewType::e2D;
		vInfo.image = image;
		vInfo.format = mSwapchainDetails.surfaceFormat.format;
		vInfo.components = {
			vk::ComponentSwizzle::eR,
			vk::ComponentSwizzle::eG,
			vk::ComponentSwizzle::eB,
			vk::ComponentSwizzle::eA,
		};
		vInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		vInfo.subresourceRange.baseMipLevel = 0;
		vInfo.subresourceRange.levelCount = 1;
		vInfo.subresourceRange.baseArrayLayer = 0;
		vInfo.subresourceRange.layerCount = 1;

		SwapchainImage Image{
			.image = image,
			.view = mDevice.createImageView(vInfo),
			.subresource = vInfo.subresourceRange,
		};
		mImages.push_back(Image);
	}

	// Rendering Info
	{
		mColorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		mColorAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
		mColorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
		mColorAttachmentInfo.clearValue.color = vk::ClearColorValue{ 0.08f, 0.08f, 0.08f, 1.0f };

		mRenderingInfo.setColorAttachments(mColorAttachmentInfo);
		mRenderingInfo.pDepthAttachment = nullptr;
		mRenderingInfo.viewMask = 0;
		mRenderingInfo.layerCount = 1;
		mRenderingInfo.renderArea.extent = mSwapchainDetails.extent;
	}

	NEXUS_LOG("Vulkan", "Created Screen Resources (Image Count - %i)", (uint32_t)mImages.size());
}

void Nexus::VulkanGraphicsDevice::DestroyScreenResources()
{
	mDevice.waitIdle();

	for (uint32_t i = 0; i < mSwapchainDetails.minImageCount; i++)
	{
		mDevice.destroyImageView(mImages[i].view);
	}
	mImages.clear();
	
	mDevice.destroySwapchainKHR(mSwapchain);
	mSwapchain = nullptr;

	NEXUS_LOG("Vulkan", "Destroyed Screen Resources");
}

extern "C"
{
	__declspec(dllexport) Nexus::GraphicsDevice* CreateGraphicsDevice()
	{
		return new Nexus::VulkanGraphicsDevice;
	}
}