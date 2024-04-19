#pragma once
#include "NxGraphics/Image.h"

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

namespace Nexus
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(const ImageSpecifications& specs);
		~VulkanImage();
	private:
		VkImage mImage;
		VkImageView mView;
		VmaAllocation mAlloc;
	};
}
