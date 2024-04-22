#include "NxVulkan/VkFramebuffer.h"
#include "NxVulkan/VkGraphicsDevice.h"
#include "NxVulkan/VkSpecification.h"

#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

Nexus::VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specs, VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice), mSpecs(specs)
{
	CreateScreenResources(specs.extent.width, specs.extent.height);
}

Nexus::VulkanFramebuffer::~VulkanFramebuffer()
{
	DestroyScreenResources();
}

void Nexus::VulkanFramebuffer::Resize(int width, int height)
{
	mDevice->WaitForGPU();
	DestroyScreenResources();
	CreateScreenResources(width, height);
}

void Nexus::VulkanFramebuffer::Begin()
{
	auto cmd = mDevice->getCommandBuffer();
	auto frameIndex = mDevice->getFrameIndex();

	for (uint32_t i = 0; i < (uint32_t) mColorAttachments[frameIndex].size(); i++)
	{
		auto& atc = mColorAttachments[frameIndex][i];
		mColorInfo[i].imageView = atc.view;

		vk::ImageMemoryBarrier barrier;
		barrier.image = atc.image;
		barrier.oldLayout = vk::ImageLayout::eUndefined;
		barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barrier.subresourceRange = atc.subresource;
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = GetAccessFlagFromLayout(vk::ImageLayout::eColorAttachmentOptimal);

		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::DependencyFlags{},
			{}, {}, barrier);

		atc.layout = barrier.newLayout;
	}

	if (mDepthAttachment.image)
	{
		vk::ImageMemoryBarrier barrier;
		barrier.image = mDepthAttachment.image;
		barrier.oldLayout = vk::ImageLayout::eUndefined;
		barrier.newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		barrier.subresourceRange = mDepthAttachment.subresource;
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = GetAccessFlagFromLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::DependencyFlags{}, 
			{}, {}, barrier);

		mDepthAttachment.layout = barrier.newLayout;
	}

	cmd.beginRendering(mInfo);
}

void Nexus::VulkanFramebuffer::End()
{
	auto cmd = mDevice->getCommandBuffer();
	cmd.endRendering();
}

void Nexus::VulkanFramebuffer::ChangeColorAttachmentLayout(vk::CommandBuffer cmdbuffer ,vk::ImageLayout newLayout, uint32_t frameIndex, uint32_t attachmentIndex, 
	vk::PipelineStageFlags srcFlag, vk::PipelineStageFlags dstFlag)
{
	auto& atc = mColorAttachments.at(frameIndex).at(attachmentIndex);

	if (atc.layout == newLayout)
		return;

	vk::ImageMemoryBarrier barrier;
	barrier.image = atc.image;
	barrier.oldLayout = atc.layout;
	barrier.newLayout = newLayout;
	barrier.subresourceRange = atc.subresource;
	barrier.srcAccessMask = GetAccessFlagFromLayout(atc.layout);
	barrier.dstAccessMask = GetAccessFlagFromLayout(newLayout);
	
	auto srcFlags = GetPipelineStageFromLayout(atc.layout);
	auto dstFlags = GetPipelineStageFromLayout(newLayout);

	cmdbuffer.pipelineBarrier(srcFlags, dstFlags, vk::DependencyFlags{}, {}, {}, barrier);
	atc.layout = newLayout;
}

void Nexus::VulkanFramebuffer::ChangeDepthAttachmentLayout(vk::CommandBuffer cmdbuffer, vk::ImageLayout newLayout,
	vk::PipelineStageFlags srcFlag, vk::PipelineStageFlags dstFlag)
{
	if (mDepthAttachment.layout == newLayout)
		return;

	vk::ImageMemoryBarrier barrier;
	barrier.image = mDepthAttachment.image;
	barrier.oldLayout = mDepthAttachment.layout;
	barrier.newLayout = newLayout;
	barrier.subresourceRange = mDepthAttachment.subresource;
	barrier.srcAccessMask = GetAccessFlagFromLayout(mDepthAttachment.layout);
	barrier.dstAccessMask = GetAccessFlagFromLayout(newLayout);

	auto srcFlags = GetPipelineStageFromLayout(mDepthAttachment.layout);
	auto dstFlags = GetPipelineStageFromLayout(newLayout);

	cmdbuffer.pipelineBarrier(srcFlags, dstFlags, vk::DependencyFlags{}, {}, {}, barrier);
	mDepthAttachment.layout = newLayout;
}

std::vector<vk::ImageView> Nexus::VulkanFramebuffer::getAttachmentViews(uint32_t attachmentIndex)
{
	std::vector<vk::ImageView> views;
	views.reserve(mColorAttachments.size());

	for (auto& atc : mColorAttachments)
	{
		views.push_back(atc.at(attachmentIndex).view);
	}

	return views;
}

void Nexus::VulkanFramebuffer::CreateScreenResources(int width, int height)
{
	mExtent.width = width;
	mExtent.height = height;
	mExtent.depth = 1;

	if (mExtent.width == 0 || mExtent.height == 0)
		mExtent = vk::Extent3D(mDevice->getExtent(), 1);

	//
	// color attachments
	//

	mColorInfo.resize(mSpecs.colorAttachments.size());

	auto imageCount = mDevice->getImageCount();
	for (uint32_t i = 0; i < imageCount; i++)
	{
		auto& attachments = mColorAttachments.emplace_back();

		for (uint64_t j = 0; j < mSpecs.colorAttachments.size(); j++)
		{
			auto& attachment = attachments.emplace_back();
			auto& info = mColorInfo[j];
			auto& specification = mSpecs.colorAttachments[j];

			CreateAttachment(AttachmentCreateInfo{
				.specs = specification,
				.extent = mExtent,
				.usage = vk::ImageUsageFlagBits::eColorAttachment,
				.aspect = vk::ImageAspectFlagBits::eColor,
				.layout = vk::ImageLayout::eColorAttachmentOptimal
				}, &attachment, i == 0 ? &info : nullptr);
		}
	}
	mInfo.setColorAttachments(mColorInfo);

	//
	// depth attachments
	//

	if (mSpecs.depthStencilAttachment.format != Nexus::ImageFormat::eUndefined)
	{
		CreateAttachment(AttachmentCreateInfo{
				.specs = mSpecs.depthStencilAttachment,
				.extent = mExtent,
				.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
				.aspect = vk::ImageAspectFlagBits::eDepth,
				.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal
			}, &mDepthAttachment, &mDepthInfo);

		mInfo.pDepthAttachment = &mDepthInfo;
	}

	// rendering info

	mInfo.viewMask = 0;
	mInfo.layerCount = 1;
	mInfo.renderArea.extent = vk::Extent2D(mExtent.width, mExtent.height);
}

void Nexus::VulkanFramebuffer::DestroyScreenResources()
{
	auto vkDevice = mDevice->getDevice();
	auto vkAllocator = mDevice->getAllocator();

	for (auto& atc : mColorAttachments)
	{
		for (auto& a : atc)
		{
			vmaDestroyImage(vkAllocator, a.image, a.alloc);
			vkDevice.destroyImageView(a.view);
		}
	}
	mColorAttachments.clear();
	mColorInfo.clear();

	if (mDepthAttachment.image)
	{
		vmaDestroyImage(vkAllocator, mDepthAttachment.image, mDepthAttachment.alloc);
		vkDevice.destroyImageView(mDepthAttachment.view);
	}
}

void Nexus::VulkanFramebuffer::CreateAttachment(const AttachmentCreateInfo& info, Attachment* pOutAttachment, vk::RenderingAttachmentInfo* pOutInfo)
{
	auto vkDevice = mDevice->getDevice();
	auto vkAllocator = mDevice->getAllocator();
	auto msaaFlagBit = mDevice->getMSAAbits();

	uint32_t arrayLayers = 1;
	uint32_t mipLevels = 1;

	vk::ImageCreateInfo imageInfo{};
	imageInfo.extent = info.extent;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.arrayLayers = arrayLayers;
	imageInfo.mipLevels = mipLevels;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | info.usage;

	imageInfo.format = info.specs.format == ImageFormat::eSwapchainColor ? mDevice->getColorFormat() : GetVulkanFormat(info.specs.format);
	imageInfo.samples = info.specs.bMultiSampled ? msaaFlagBit : vk::SampleCountFlagBits::e1;

	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.format = imageInfo.format;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };
	viewInfo.subresourceRange.aspectMask = info.aspect;
	viewInfo.subresourceRange.layerCount = arrayLayers;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.baseMipLevel = 0;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VkImage image{};
	VmaAllocation allocation{};

	try
	{
		VkImageCreateInfo imageCreateInfo = imageInfo;
		vk::resultCheck((vk::Result)vmaCreateImage(vkAllocator, &imageCreateInfo, &allocInfo, &image, &allocation, nullptr), "Failed to create vma Image");
	}
	catch (vk::SystemError& er)
	{
		NEXUS_ASSERT(true, "VMA Error: %s", er.what());
	}

	viewInfo.image = image;

	pOutAttachment->image = image;
	pOutAttachment->alloc = allocation;
	pOutAttachment->view = vkDevice.createImageView(viewInfo);
	pOutAttachment->layout = imageInfo.initialLayout;
	pOutAttachment->subresource = viewInfo.subresourceRange;
	
	if (pOutInfo)
	{
		pOutInfo->imageView = pOutAttachment->view;
		pOutInfo->imageLayout = info.layout;
		pOutInfo->loadOp = GetVulkanLoadOp(info.specs.loadOp);
		pOutInfo->storeOp = GetVulkanStoreOp(info.specs.storeOp);
		pOutInfo->clearValue = GetVulkanClearValue(info.specs.clearValue);
	}
}
