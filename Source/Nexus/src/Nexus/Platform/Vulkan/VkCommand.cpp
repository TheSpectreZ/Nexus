#include "nxpch.h"
#include "VkCommand.h"

#include "Renderer/Renderer.h"
#include "VkRenderpass.h"
#include "VkFramebuffer.h"
#include "VkBuffer.h"
#include "VkPipeline.h"

void Nexus::VulkanCommand::ImplInit()
{
	m_TransferQueue = DynamicPointerCast<VulkanTransferCommandQueue>(Renderer::GetTransferCommandQueue());
	m_RenderQueue = DynamicPointerCast<VulkanRenderCommandQueue>(Renderer::GetRenderCommandQueue());
}

void Nexus::VulkanCommand::ImplUpdate()
{
	m_RenderCommandBuffer = m_RenderQueue->GetCurrentCommandBuffer();
	m_FrameIndex = m_RenderQueue->GetFrameIndex();
}

void Nexus::VulkanCommand::ImplBeginRenderpass(Ref<Renderpass> r, Ref<Framebuffer> f)
{
	Ref<VulkanRenderpass> renderpass = DynamicPointerCast<VulkanRenderpass>(r);
	Ref<VulkanFramebuffer> framebuffer = DynamicPointerCast<VulkanFramebuffer>(f);

	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = renderpass->Get();
	Info.framebuffer = framebuffer->Get(m_FrameIndex);
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = framebuffer->GetExtent();
	Info.clearValueCount = (uint32_t)framebuffer->GetClearValues().size();
	Info.pClearValues = framebuffer->GetClearValues().data();

	vkCmdBeginRenderPass(m_RenderCommandBuffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::VulkanCommand::ImplEndRenderpass()
{
	vkCmdEndRenderPass(m_RenderCommandBuffer);
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
