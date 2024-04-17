#pragma once
#include "NxGraphics/GraphicsDevice.h"

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "NxVulkan/VkFramebuffer.h"

namespace Nexus
{
	class VulkanGraphicsDevice : public GraphicsDevice
	{
	public:
		bool Initialize(const GraphicsDeviceSpecifications& specs) override;
		void Shutdown() override;
		void WaitForGPU() override;

		void BeginFrame() override;
		void EndFrame(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) override;
		
		void Flush() override;

		void OnScreenResize(int width, int height) override;
	public:
		Extent getDeviceExtent() override 
		{
			return { mSwapchainDetails.extent.width, mSwapchainDetails.extent.height }; 
		} 
	public:
		vk::Device getDevice() { return mDevice; }
		VmaAllocator getAllocator() { return mAllocator; }

		vk::CommandBuffer getCommandBuffer() { return mGraphicsCommandBuffer[mFrameIndex]; }

		uint32_t getImageCount() { return mSwapchainDetails.minImageCount; }
		uint32_t getFrameIndex() { return mFrameIndex; }

		vk::Extent2D getExtent() { return mSwapchainDetails.extent; }
		vk::SampleCountFlagBits getMSAAbits() { return mMaxSampleCount; }
	public:
		inline Ref< Framebuffer > CreateFramebuffer(const FramebufferSpecification& specs) override 
		{
			return CreateRef<VulkanFramebuffer>(specs, this); 
		}
	private:
		vk::Instance mInstance;
		vk::DebugUtilsMessengerEXT mDebugMessenger;

		vk::PhysicalDevice mPhysicalDevice;
		vk::PhysicalDeviceProperties mPhysicalDeviceProps;

		vk::Format mDepthFormat;
		vk::SampleCountFlagBits mMaxSampleCount;

		vk::Device mDevice;

		vk::Queue mGraphicsQueue;
		vk::Queue mTransferQueue;
		vk::Queue mComputeQueue;

		VmaAllocator mAllocator;

		vk::SurfaceKHR mSurface;
		vk::SwapchainKHR mSwapchain;

		std::vector<vk::Fence> mFence;
		std::vector<vk::Semaphore> mImageAvailSemaphore;
		std::vector<vk::Semaphore> mRenderFinishedSemaphore;
			
		vk::CommandPool mGraphicsCommandPool;
		std::vector<vk::CommandBuffer> mGraphicsCommandBuffer;

		vk::RenderingAttachmentInfo mColorAttachmentInfo;
		vk::RenderingInfo mRenderingInfo;
	private:
		Window* mWindow;

		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Transfer = -1;
			int32_t Compute = -1;
		} mQueueFamIndices;

		QueueFamilyIndices QueryQueueFamilyIndices(vk::QueueFlags flags);

		struct SwapchainDetails
		{
			vk::SurfaceFormatKHR surfaceFormat;
			vk::PresentModeKHR presentMode;
			
			VkExtent2D extent;
			uint32_t minImageCount;
		} mSwapchainDetails;

		SwapchainDetails QuerySwapchainDetails();

		struct SwapchainImage
		{
			vk::Image image;
			vk::ImageView view;
			vk::ImageSubresourceRange subresource;
		};
		std::vector< SwapchainImage > mImages;

		void CreateScreenResources(int width, int height);
		void DestroyScreenResources();

		uint32_t mFrameIndex = 0;
		uint32_t mImageIndex = 0;
	};
}