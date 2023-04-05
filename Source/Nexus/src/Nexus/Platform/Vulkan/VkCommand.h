#pragma once
#include "VkAssert.h"
#include "Renderer/Command.h"
#include "VkTransferCommandQueue.h"

namespace Nexus
{
	class VulkanCommand : public Command
	{
		void ImplInit() override;
		void ImplUpdate() override;
		
		void ImplTransferStaticMesh(Ref<StaticMesh> mesh) override;

		void ImplBindPipeline(Ref<Pipeline> pipeline) override;

		void ImplDrawMesh(Ref<StaticMesh> mesh) override;
		
		void ImplSetViewport(const Viewport& viewport) override;
		void ImplSetScissor(const Scissor& scissor) override;
		
		Ref<VulkanTransferCommandQueue> m_TransferQueue;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
	public:
		VkCommandBuffer m_RenderCommandBuffer;
	};
}

