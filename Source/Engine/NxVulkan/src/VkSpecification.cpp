#include "NxVulkan/VkSpecification.h"

vk::Format Nexus::GetVulkanFormat(ImageFormat format)
{
	switch (format)
	{
	case Nexus::ImageFormat::eRGBA8_SFLOAT:
		return vk::Format::eR8G8B8A8Srgb;
	case Nexus::ImageFormat::eD32_SFLOAT:
		return vk::Format::eD32Sfloat;
	default: 
		return vk::Format::eUndefined;
	}
}

vk::AttachmentLoadOp Nexus::GetVulkanLoadOp(ImageLoadOp op)
{
	switch (op)
	{
	case Nexus::ImageLoadOp::eClear:
		return vk::AttachmentLoadOp::eClear;
	case Nexus::ImageLoadOp::eLoad:
		return vk::AttachmentLoadOp::eLoad;
	case Nexus::ImageLoadOp::eDontCare:
		return vk::AttachmentLoadOp::eDontCare;
	default:
		return vk::AttachmentLoadOp::eNoneEXT;
	}
}

vk::AttachmentStoreOp Nexus::GetVulkanStoreOp(ImageStoreOp op)
{
	switch (op)
	{
	case Nexus::ImageStoreOp::eStore:
		return vk::AttachmentStoreOp::eStore;
	case Nexus::ImageStoreOp::eDontCare:
		return vk::AttachmentStoreOp::eDontCare;
	default:
		return vk::AttachmentStoreOp::eNoneEXT;
	}
}

vk::ImageLayout Nexus::GetVulkanImageLayout(ImageLayout layout)
{
	switch (layout)
	{
	case Nexus::ImageLayout::eColorAttachment:
		return vk::ImageLayout::eColorAttachmentOptimal;
	case Nexus::ImageLayout::eDepthStencilAttachment:
		return vk::ImageLayout::eDepthStencilAttachmentOptimal;
	case Nexus::ImageLayout::eTransferSrc:
		return vk::ImageLayout::eTransferSrcOptimal;
	case Nexus::ImageLayout::eTransferDst:
		return vk::ImageLayout::eTransferDstOptimal;
	default:
		return vk::ImageLayout::eUndefined;
	}
}


vk::ClearValue Nexus::GetVulkanClearValue(ImageClearValue value)
{
	vk::ClearValue vkValue;

	if (value.index() == 0)
	{
		auto& val = std::get<ImageClearColorValue>(value);
		vkValue.color = vk::ClearColorValue{ val.rgba };
	}
	else
	{
		auto& val = std::get<ImageClearDepthStencilValue>(value);
		vkValue.depthStencil = vk::ClearDepthStencilValue{ val.depth, val.stencil };
	}

	return vkValue;
}

vk::AccessFlagBits Nexus::GetAccessFlagFromLayout(vk::ImageLayout layout)
{
	switch (layout)
	{
	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::AccessFlagBits::eColorAttachmentWrite;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	case vk::ImageLayout::eTransferSrcOptimal:
		return vk::AccessFlagBits::eTransferRead;
	case vk::ImageLayout::eTransferDstOptimal:
		return vk::AccessFlagBits::eTransferWrite;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::AccessFlagBits::eShaderRead;
	default:
		return vk::AccessFlagBits::eNone;
	}
}

vk::PipelineStageFlagBits Nexus::GetPipelineStageFromLayout(vk::ImageLayout layout)
{
	switch (layout)
	{
	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::PipelineStageFlagBits::eColorAttachmentOutput;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::PipelineStageFlagBits::eEarlyFragmentTests;
	case vk::ImageLayout::eTransferSrcOptimal:
		return vk::PipelineStageFlagBits::eTransfer;
	case vk::ImageLayout::eTransferDstOptimal:
		return vk::PipelineStageFlagBits::eTransfer;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::PipelineStageFlagBits::eFragmentShader;
	default:
		return vk::PipelineStageFlagBits::eNone;
	}
}
