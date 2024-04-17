#pragma once
#include "NxGraphics/Specification.h"

#include "vulkan/vulkan.hpp"

namespace Nexus
{
	vk::Format GetVulkanFormat(ImageFormat format);

	vk::AttachmentLoadOp GetVulkanLoadOp(ImageLoadOp op);
	vk::AttachmentStoreOp GetVulkanStoreOp(ImageStoreOp op);

	vk::ImageLayout GetVulkanImageLayout(ImageLayout layout);

	vk::ClearValue GetVulkanClearValue(ImageClearValue value);

	vk::AccessFlagBits GetAccessFlagFromLayout(vk::ImageLayout layout);

	vk::PipelineStageFlagBits GetPipelineStageFromLayout(vk::ImageLayout layout);
}
