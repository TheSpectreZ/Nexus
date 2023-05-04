#pragma once
#include "VkAssert.h"
#include "Renderer/Framebuffer.h"

namespace Nexus
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& specs);
		~VulkanFramebuffer() override;

		VkFramebuffer& Get(uint32_t frameIndex) { return m_Framebuffer[frameIndex]; }
		VkImageView& GetAttachmentView(uint32_t frameIndex, uint32_t attachmentIndex) { return m_Attachments[frameIndex][attachmentIndex].view; }

		std::vector<VkClearValue>& GetClearValues() { return m_ClearValues; }
		VkExtent2D& GetExtent() { return m_Extent; }
	private:
		std::vector<VkFramebuffer> m_Framebuffer;
		
		struct Attachment
		{
			VkImage image;
			VkImageView view;
			VmaAllocation mem;

			void Create(const FramebufferAttachmentDescription& desc);
			void Destroy(VkDevice device,VmaAllocator allocator);
		};
		std::vector<std::vector<Attachment>> m_Attachments;

		std::vector<VkClearValue> m_ClearValues;
		VkExtent2D m_Extent;
	};
}


