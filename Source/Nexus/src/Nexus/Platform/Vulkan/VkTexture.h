#pragma once
#include "VkAssert.h"
#include "Renderer/Texture.h"

namespace Nexus
{
	class VulkanTexture : public Texture
	{
		friend class VulkanCommandQueue;
	public:
		VulkanTexture(const TextureCreateInfo& Info);
		~VulkanTexture() override;

		VkImageView Get() { return m_View; }
	private:
		Extent m_Extent;

		VkImage m_Image;
		VkImageView m_View;
		VmaAllocation m_Alloc;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAlloc;
	};

	class VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W);
		~VulkanSampler() override;

		VkSampler Get() { return m_Sampler; }
	private:
		VkSampler m_Sampler;
	};
}
