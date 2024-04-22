#pragma once
#include "NxGraphics/Framebuffer.h"

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

namespace Nexus
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& specs, class VulkanGraphicsDevice* pDevice);
		~VulkanFramebuffer() override;

		void Resize(int width, int height) override;

		void Begin() override;
		void End() override;

		void ChangeColorAttachmentLayout(vk::CommandBuffer cmdbuffer, vk::ImageLayout newLayout, uint32_t frameIndex, uint32_t attachmentIndex, 
			vk::PipelineStageFlags srcFlag = vk::PipelineStageFlagBits::eNone,
			vk::PipelineStageFlags dstFlag = vk::PipelineStageFlagBits::eNone);

		void ChangeDepthAttachmentLayout(vk::CommandBuffer cmdbuffer, vk::ImageLayout newLayout,
			vk::PipelineStageFlags srcFlag = vk::PipelineStageFlagBits::eNone,
			vk::PipelineStageFlags dstFlag = vk::PipelineStageFlagBits::eNone);
		
		struct Attachment
		{
			vk::Image image;
			vk::ImageView view;
			VmaAllocation alloc;

			vk::ImageLayout layout;
			
			vk::ImageSubresourceRange subresource;
		};
		
		inline Attachment getAttachment(uint32_t frameIndex, uint32_t attachmentIndex) 
		{ 
			return mColorAttachments.at(frameIndex).at(attachmentIndex);
		}

		inline vk::Extent3D getExtent()
		{
			return mExtent;
		}

		std::vector<vk::ImageView> getAttachmentViews(uint32_t attachmentIndex);
	private:
		class VulkanGraphicsDevice* mDevice;

		FramebufferSpecification mSpecs;

		vk::Extent3D mExtent;
		vk::RenderingInfo mInfo;

		std::vector< std::vector<Attachment> > mColorAttachments;
		std::vector< vk::RenderingAttachmentInfo > mColorInfo;
		
		Attachment mDepthAttachment;
		vk::RenderingAttachmentInfo mDepthInfo;
	private:
		struct AttachmentCreateInfo
		{
			FramebufferAttachment specs;
			vk::Extent3D extent;
			
			vk::ImageUsageFlagBits usage;
			vk::ImageAspectFlagBits aspect;
			vk::ImageLayout layout;
		};

		void CreateScreenResources(int width, int height);
		void DestroyScreenResources();
		
		void CreateAttachment(const AttachmentCreateInfo& info, Attachment* pOutAttachment, vk::RenderingAttachmentInfo* pOutInfo);
	};
}
