#include "NxVulkan/VkFramebuffer.h"
#include "NxVulkan/VkGraphicsDevice.h"
#include "NxVulkan/VkSpecification.h"

#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

Nexus::VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specs, VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice)
{
	mExtent.width = specs.extent.width;
	mExtent.height = specs.extent.height;
	mExtent.depth = 1;
	
	if (mExtent.width == 0 || mExtent.height == 0)
		mExtent = vk::Extent3D(pDevice->getExtent(), 1);

	//
	// color attachments
	//

	mColorInfo.resize(specs.colorAttachments.size());
	
	auto imageCount = pDevice->getImageCount();
	for (uint32_t i = 0; i < imageCount; i++)
	{
		auto& attachments = mColorAttachments.emplace_back();
	
		for (uint64_t j = 0; j < specs.colorAttachments.size(); j++)
		{
			auto& attachment = attachments.emplace_back();
			auto& info = mColorInfo[j];
			auto& specification = specs.colorAttachments[j];

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

	if (specs.depthStencilAttachment.format != Nexus::ImageFormat::eUndefined)
	{
		CreateAttachment(AttachmentCreateInfo{
				.specs = specs.depthStencilAttachment,
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

Nexus::VulkanFramebuffer::~VulkanFramebuffer()
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
	
	if (mDepthAttachment.image)
	{
		vmaDestroyImage(vkAllocator, mDepthAttachment.image, mDepthAttachment.alloc);
		vkDevice.destroyImageView(mDepthAttachment.view);
	}
}

void Nexus::VulkanFramebuffer::Begin()
{
	auto cmd = mDevice->getCommandBuffer();
	auto frameIndex = mDevice->getFrameIndex();

	for (uint64_t i = 0; i < mColorAttachments[frameIndex].size(); i++)
	{
		auto& atc = mColorAttachments[frameIndex][i];
		mColorInfo[i].imageView = atc.view;

		vk::ImageMemoryBarrier imageBarrier;

		imageBarrier.oldLayout = vk::ImageLayout::eUndefined;
		imageBarrier.newLayout = atc.layout;
		imageBarrier.image = atc.image;
		imageBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
		imageBarrier.dstAccessMask = GetAccessFlagFromLayout(atc.layout);
		imageBarrier.subresourceRange = atc.subresource;

		auto stageFlag = GetPipelineStageFromLayout(atc.layout);

		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, stageFlag, vk::DependencyFlags{}, {}, {}, imageBarrier);
	}

	if (mDepthAttachment.image)
	{
		vk::ImageMemoryBarrier imageBarrier;
		imageBarrier.oldLayout = vk::ImageLayout::eUndefined;
		imageBarrier.newLayout = mDepthAttachment.layout;
		imageBarrier.image = mDepthAttachment.image;
		imageBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
		imageBarrier.dstAccessMask = GetAccessFlagFromLayout(mDepthAttachment.layout);
		imageBarrier.subresourceRange = mDepthAttachment.subresource;
		
		auto stageFlag = GetPipelineStageFromLayout(mDepthAttachment.layout);
		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, stageFlag, vk::DependencyFlags{}, {}, {}, imageBarrier);
	}

	cmd.beginRendering(mInfo);
}

void Nexus::VulkanFramebuffer::End()
{
	auto cmd = mDevice->getCommandBuffer();
	cmd.endRendering();
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

	imageInfo.format = GetVulkanFormat(info.specs.format);
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
	pOutAttachment->layout = info.layout;
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
