#pragma once
#include "Editor/EditorContext.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanEditorContext : public EditorContext
	{
		friend class VulkanEditorViewport;
	public:
		VulkanEditorContext(Ref<Renderpass> renderpass);
		~VulkanEditorContext();
	private:
		void Start() override;
		void End() override;

		ImTextureID MakeTextureID(Ref<Framebuffer> framebuffer, Ref<Sampler> sampler, uint32_t bufferIndex, uint32_t imageIndex);
		ImTextureID MakeTextureID(Ref<Texture> texture, Ref<Sampler> sampler) override;
		void DestroyTextureID(ImTextureID Id);

		void BindTextureID(ImTextureID Id) override;

		VkDescriptorPool m_Pool;
		VkCommandBuffer m_CmdBuffer;

		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_SetLayout;
		std::vector<VkDescriptorSet> m_Descriptors;
	};
}
