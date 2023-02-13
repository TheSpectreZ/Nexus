#include "Graphics/Renderpass.h"
#include "Graphics/Engine.h"
#include "vkAssert.h"

void Nexus::Graphics::Renderpass::Create(const RenderpassCreateInfo& Info)
{
	VkRenderPassCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	
	info.pAttachments = Info.descriptions.data();
	info.pDependencies = Info.dependecies.data();
	info.pSubpasses = Info.subpasses.data();

	info.attachmentCount = (uint32_t)Info.descriptions.size();
	info.dependencyCount = (uint32_t)Info.dependecies.size();
	info.subpassCount = (uint32_t)Info.subpasses.size();

	_VKR = vkCreateRenderPass(Engine::Get().GetDevice(), &info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkRenderPass);
	NEXUS_LOG_TRACE("Renderpass Created");

}

void Nexus::Graphics::Renderpass::Destroy()
{
	vkDestroyRenderPass(Engine::Get().GetDevice(), m_handle, nullptr);
	NEXUS_LOG_TRACE("Renderpass Destroyed");
}

void Nexus::Graphics::Renderpass::Begin(VkCommandBuffer cmd)
{
	VkRenderPassBeginInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Info.pNext = nullptr;
	Info.renderPass = m_handle;
	Info.framebuffer = m_CurrentFramebuffers->at(Engine::Get().GetCurrentFrameIndex()).Get();
	Info.renderArea.offset = { 0,0 };
	Info.renderArea.extent = Engine::Get().GetSwapchainImageExtent();
	Info.clearValueCount = (uint32_t)m_CurrentClearValues->size();
	Info.pClearValues = m_CurrentClearValues->data();

	vkCmdBeginRenderPass(cmd, &Info, VK_SUBPASS_CONTENTS_INLINE);
}

void Nexus::Graphics::Renderpass::End(VkCommandBuffer cmd)
{
	vkCmdEndRenderPass(cmd);
}