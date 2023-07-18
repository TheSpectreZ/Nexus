#pragma once
#include "VkAssert.h"
#include "NxGraphics/Texture.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanTexture : public Texture
	{
		friend class VulkanCommandQueue;
	public:
		VulkanTexture(const TextureSpecification& Specs);
		~VulkanTexture() override;

		void PrepareForRender() override;

		VkImageView Get(uint32_t index = 0) { return m_Views[index]; }
	private:
		uint32_t m_ArrayLayerCount, m_MipCount;
		Extent m_Extent;
		VkImageLayout m_CurrentLayout;

		VkImage m_Image;
		std::vector<VkImageView> m_Views;
		VmaAllocation m_Alloc;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAlloc;
	};

	class NEXUS_VULKAN_API VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(const SamplerSpecification& Specs);
		~VulkanSampler() override;

		VkSampler Get() { return m_Sampler; }
	private:
		VkSampler m_Sampler;
	};
}
