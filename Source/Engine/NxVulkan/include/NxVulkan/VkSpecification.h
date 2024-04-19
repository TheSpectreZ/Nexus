#pragma once
#include "NxGraphics/Specification.h"

#include "vulkan/vulkan.hpp"

namespace Nexus
{
	// NOTE: Returns vk::Format::eUndefined for ImageFormat::eSwapchainColor
	// Make sure you manually check that case and retrieve the correct format from VulkanGraphicsDevice
	vk::Format GetVulkanFormat(ImageFormat format);

	vk::AttachmentLoadOp GetVulkanLoadOp(ImageLoadOp op);
	vk::AttachmentStoreOp GetVulkanStoreOp(ImageStoreOp op);

	vk::ImageLayout GetVulkanImageLayout(ImageLayout layout);

	vk::ClearValue GetVulkanClearValue(ImageClearValue value);

	vk::AccessFlagBits GetAccessFlagFromLayout(vk::ImageLayout layout);

	vk::PipelineStageFlagBits GetPipelineStageFromLayout(vk::ImageLayout layout);
}
