#include "nxpch.h"
#include "VkCommand.h"

#include "Renderer/Renderer.h"
#include "VkRenderCommandQueue.h"
#include "VkTransferCommandQueue.h"
#include "VkBuffer.h"
#include "VkPipeline.h"

void Nexus::VulkanCommand::ImplInit()
{
	m_TransferQueue = DynamicPointerCast<VulkanTransferCommandQueue>(Renderer::GetTransferCommandQueue());
}

void Nexus::VulkanCommand::ImplUpdate()
{
	m_RenderCommandBuffer = DynamicPointerCast<VulkanRenderCommandQueue>(Renderer::GetRenderCommandQueue())->GetCurrentCommandBuffer();
}

void Nexus::VulkanCommand::ImplTransferStaticMesh(Ref<StaticMesh> mesh)
{
	if (mesh == nullptr)
		NEXUS_LOG_DEBUG("Mesh Ptr is Null");

	m_TransferQueue->PushStaticBuffer(DynamicPointerCast<VulkanStaticBuffer>(mesh->GetVertexBuffer()));
	m_TransferQueue->PushStaticBuffer(DynamicPointerCast<VulkanStaticBuffer>(mesh->GetIndexBuffer()));
}

void Nexus::VulkanCommand::ImplBindPipeline(Ref<Pipeline> pipeline)
{
	Ref<VulkanPipeline> Vkp = DynamicPointerCast<VulkanPipeline>(pipeline);
	vkCmdBindPipeline(m_RenderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Vkp->Get());
}

void Nexus::VulkanCommand::ImplDrawMesh(Ref<StaticMesh> mesh)
{
	Ref<VulkanStaticBuffer> vb = DynamicPointerCast<VulkanStaticBuffer>(mesh->GetVertexBuffer());
	Ref<VulkanStaticBuffer> ib = DynamicPointerCast<VulkanStaticBuffer>(mesh->GetIndexBuffer());

	VkBuffer buf[] = { vb->Get() };
	VkDeviceSize off[] = { 0 };

	vkCmdBindVertexBuffers(m_RenderCommandBuffer, 0, 1, buf, off);
	vkCmdBindIndexBuffer(m_RenderCommandBuffer, ib->Get(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(m_RenderCommandBuffer, ib->m_size/sizeof(uint32_t), 1, 0, 0, 0);
}

void Nexus::VulkanCommand::ImplSetViewport(const Viewport& viewport)
{
	m_Viewport.x = viewport.x;
	m_Viewport.y = viewport.y;
	m_Viewport.width = viewport.width;
	m_Viewport.height = viewport.height;
	m_Viewport.minDepth = viewport.minDepth;
	m_Viewport.maxDepth = viewport.maxDepth;

	vkCmdSetViewport(m_RenderCommandBuffer, 0, 1, &m_Viewport);
}

void Nexus::VulkanCommand::ImplSetScissor(const Scissor& scissor)
{
	m_Scissor.offset = { scissor.Offset.x, scissor.Offset.y };
	m_Scissor.extent = { scissor.Extent.width,scissor.Extent.height };

	vkCmdSetScissor(m_RenderCommandBuffer, 0, 1, &m_Scissor);
}

void Nexus::VulkanCommand::ImplSetPushConstantData(Ref<Pipeline> pipeline, void* data, uint32_t size)
{
	Ref<VulkanPipeline> Vkp = DynamicPointerCast<VulkanPipeline>(pipeline);
	vkCmdPushConstants(m_RenderCommandBuffer, Vkp->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT , 0, size, data);
}
