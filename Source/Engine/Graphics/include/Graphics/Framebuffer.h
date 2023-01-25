#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"

#include "Renderpass.h"

#include <vector>

namespace Nexus
{
	namespace Graphics
	{
		VkFormat NEXUS_GRAPHICS_API GetSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		struct NEXUS_GRAPHICS_API FramebufferAttachmentCreateInfo
		{
			VkExtent2D extent;
			VkFormat format;
			VkImageUsageFlags usage;
			VkSampleCountFlagBits samples;
			VkImageAspectFlagBits aspect;
		};

		class NEXUS_GRAPHICS_API FramebufferAttachment
		{
		public:
			void Create(const FramebufferAttachmentCreateInfo& Info);
			void Destroy();
			VkImageView& Get() { return m_view; }

			VkFormat GetFormat() { return m_format; }
		private:
			VkFormat m_format;

			VkImage m_image;
			VkImageView m_view;
			VkDeviceMemory m_memory;
		};

		struct NEXUS_GRAPHICS_API FramebufferCreateInfo
		{
			std::vector<VkImageView> Attachments;
			Renderpass* Renderpass;
			VkExtent2D extent;
		};

		class NEXUS_GRAPHICS_API Framebuffer
		{
		public:
			void Create(const FramebufferCreateInfo& Info);
			void Destroy();

			VkFramebuffer& Get() { return m_handle; }
			VkFramebuffer Get() const { return m_handle; }
		private:
			VkFramebuffer m_handle;
		};
	}
}