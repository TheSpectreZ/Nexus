#include "nxpch.h"
#include "VkRenderCommand.h"

#include "Renderer/Renderer.h"
#include "VkRenderCommandQueue.h"

#include "VkPipeline.h"

void Nexus::VulkanRenderCommand::ImplUpdate()
{
	m_CommandBuffer = DynamicPointerCast<VulkanRenderCommandQueue>(Renderer::GetRenderCommandQueue())->GetCurrentCommandBuffer();
}

void Nexus::VulkanRenderCommand::ImplBindPipeline(Ref<Pipeline> pipeline)
{
	Ref<VulkanPipeline> Vkp = DynamicPointerCast<VulkanPipeline>(pipeline);
	vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Vkp->Get());
}

void Nexus::VulkanRenderCommand::ImplSetViewport(const Viewport& viewport)
{
	m_Viewport.x = viewport.x;
	m_Viewport.y = viewport.y;
	m_Viewport.width = viewport.width;
	m_Viewport.height = viewport.height;
	m_Viewport.minDepth = viewport.minDepth;
	m_Viewport.maxDepth = viewport.maxDepth;

	vkCmdSetViewport(m_CommandBuffer, 0, 1, &m_Viewport);
}

void Nexus::VulkanRenderCommand::ImplSetScissor(const Scissor& scissor)
{
	m_Scissor.offset = { scissor.Offset.x, scissor.Offset.y };
	m_Scissor.extent = { scissor.Extent.width,scissor.Extent.height };

	vkCmdSetScissor(m_CommandBuffer, 0, 1, &m_Scissor);
}

void Nexus::VulkanRenderCommand::ImplDraw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	vkCmdDraw(m_CommandBuffer, VertexCount, InstanceCount, FirstVertex, FirstInstance);
}