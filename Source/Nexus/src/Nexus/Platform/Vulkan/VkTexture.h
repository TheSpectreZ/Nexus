#pragma once
#include "VkAssert.h"
#include "Renderer/Texture.h"

namespace Nexus
{
	class VulkanTexture : public Texture
	{
		friend class VulkanCommand;
		friend class VulkanTransferCommandQueue;
	public:
		VulkanTexture(const TextureCreateInfo& Info);
		~VulkanTexture() override;
	private:
		Extent m_Extent;

		VkImage m_Image;
		VkImageView m_View;
		VmaAllocation m_Alloc;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAlloc;
	};
}
