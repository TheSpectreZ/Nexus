#include "Graphics/Renderpass.h"
#include "Backend.h"
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

	_VKR = vkCreateRenderPass(Backend::GetDevice(), &info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkRenderPass);
	NEXUS_LOG_TRACE("Renderpass Created");

}

void Nexus::Graphics::Renderpass::Destroy()
{
	vkDestroyRenderPass(Backend::GetDevice(), m_handle, nullptr);
	NEXUS_LOG_TRACE("Renderpass Destroyed");
}
