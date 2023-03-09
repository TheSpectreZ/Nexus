#pragma once
#include "VkAssert.h"
#include "Renderer/RenderCommand.h"

namespace Nexus
{
	class VulkanRenderCommand : public RenderCommand
	{
		void ImplUpdate() override;
		
		void ImplBindPipeline(Ref<Pipeline> pipeline) override;
		
		void ImplSetViewport(const Viewport& viewport) override;
		void ImplSetScissor(const Scissor& scissor) override;
		
		void ImplDraw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) override;
		
		VkCommandBuffer m_CommandBuffer;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
	};
}

