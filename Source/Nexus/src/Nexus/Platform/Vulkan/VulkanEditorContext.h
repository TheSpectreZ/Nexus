#pragma once
#include "Editor/EditorContext.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanEditorContext : public EditorContext
	{
		friend class VulkanEditorViewport;
	public:
		VulkanEditorContext(Ref<Renderpass> rednerpass);
		~VulkanEditorContext();
	private:
		void Start() override;
		void End() override;

		VkDescriptorPool m_Pool;
		VkCommandBuffer m_CmdBuffer;
	};
}
