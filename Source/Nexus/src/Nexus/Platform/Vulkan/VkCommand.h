#pragma once
#include "VkAssert.h"
#include "Renderer/Command.h"

#include "VkRenderCommandQueue.h"
#include "VkTransferCommandQueue.h"

namespace Nexus
{
	class VulkanCommand : public Command
	{
		void ImplInit() override;
		void ImplUpdate() override;
		
		void ImplBeginRenderpass(Ref<Renderpass> r, Ref<Framebuffer> f);
		void ImplEndRenderpass();

		void ImplTransferStaticMesh(Ref<StaticMesh> mesh) override;
		void ImplTransferMaterial(Ref<Material> material) override;

		void ImplBindPipeline(Ref<Pipeline> pipeline) override;

		void ImplDrawMesh(Ref<StaticMesh> mesh) override;
		
		void ImplSetViewport(const Viewport& viewport) override;
		void ImplSetScissor(const Scissor& scissor) override;
		
		Ref<VulkanTransferCommandQueue> m_TransferQueue;
		Ref<VulkanRenderCommandQueue> m_RenderQueue;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
	public:
		uint32_t m_FrameIndex;
		VkCommandBuffer m_RenderCommandBuffer;
	};
}

