#include "Viewport.h"
#include "NxImGui/Context.h"
#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/ResourcePool.h"

#include "NxVulkan/VkTexture.h"		
#include "NxVulkan/VkFramebuffer.h"
#include "backends/imgui_impl_vulkan.h"

void NexusEd::Viewport::Initialize()
{
	Nexus::SamplerSpecification samplerSpecs{};
	samplerSpecs.Far = Nexus::SamplerFilter::Linear;
	samplerSpecs.Near = Nexus::SamplerFilter::Linear;
	samplerSpecs.U = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.V = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.W = Nexus::SamplerWrapMode::Repeat;

	m_Sampler = Nexus::ResourcePool::Get()->GetSampler(samplerSpecs);

	m_TextureIDs.resize(Nexus::Module::Renderer::Get()->GetSwapchain()->GetImageCount());
}

void NexusEd::Viewport::SetContext(Nexus::Ref<Nexus::Framebuffer> framebuffer, uint32_t attachmentIndex)
{
	m_Framebuffer = framebuffer;
	m_Extent = Nexus::Module::Renderer::Get()->GetSwapchain()->GetExtent();

	for (uint32_t i = 0; i < m_TextureIDs.size(); i++)
	{
		NxImGui::Context::DestroyTextureID((VkDescriptorSet)m_TextureIDs[i]);

		auto samp = Nexus::DynamicPointerCast<Nexus::VulkanSampler>(m_Sampler);
		VkImageView view = Nexus::DynamicPointerCast<Nexus::VulkanFramebuffer>(m_Framebuffer)->GetAttachmentView(i, attachmentIndex);

		m_TextureIDs[i] = NxImGui::Context::CreateTextureID((void*)view, samp);
	}
}

void NexusEd::Viewport::Render()
{
	uint32_t frameIndex = NxImGui::Context::GetFrameIndex();

	NxImGui::Context::BindTextureID(m_TextureIDs[frameIndex]);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	
	ImGui::Begin("Viewport");
	
	m_Panelsize = ImGui::GetContentRegionAvail();
	ImGui::Image(m_TextureIDs[frameIndex], m_Panelsize);
	
	ImGui::End();

	ImGui::PopStyleVar();
}

glm::vec2 NexusEd::Viewport::GetViewportSize()
{
	return { m_Panelsize.x,m_Panelsize.y };
}
