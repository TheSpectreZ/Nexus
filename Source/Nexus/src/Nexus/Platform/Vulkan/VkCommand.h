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
		
		void ImplSetPushConstantData(Ref<Pipeline> pipeline, void* data, uint32_t size) override;

		void ImplDraw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) override;
		void ImplDrawIndexed(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance) override;

		Ref<VulkanTransferCommandQueue> m_TransferQueue;

		VkCommandBuffer m_RenderCommandBuffer;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
	};
}

