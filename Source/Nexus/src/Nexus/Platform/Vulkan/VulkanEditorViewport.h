#pragma once
#include "Editor/EditorViewport.h"
#include "VkFramebuffer.h"
#include "VkCommand.h"
#include "VkSwapchain.h"

namespace Nexus
{
	class VulkanEditorViewport : public EditorViewport
	{
	public:
		VulkanEditorViewport();
		~VulkanEditorViewport() override;

		void SetContext(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) override;
		void Render() override;
	private:
		Ref<VulkanFramebuffer> m_Framebuffer;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		VkSampler m_Sampler;
		VkExtent2D m_Extent;
		VkPipelineLayout* m_layout;
		Ref<VulkanSwapchain> m_Swapchain;
		Ref<VulkanCommand> m_Command;
	};
}


