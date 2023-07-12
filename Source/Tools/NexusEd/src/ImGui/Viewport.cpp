#include "Viewport.h"
#include "Context.h"
#include "NxRenderer/Renderer.h"
#include "NxRenderer/ResourcePool.h"

#include "NxVulkan/VkTexture.h"		
#include "NxVulkan/VkFramebuffer.h"
#include "backends/imgui_impl_vulkan.h"

void NexusEd::Viewport::Initialize()
{
	Nexus::SamplerSpecification samplerSpecs{};
	samplerSpecs.sampler.Far = Nexus::SamplerFilter::Linear;
	samplerSpecs.sampler.Near = Nexus::SamplerFilter::Linear;
	samplerSpecs.sampler.U = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.sampler.V = Nexus::SamplerWrapMode::Repeat;
	samplerSpecs.sampler.W = Nexus::SamplerWrapMode::Repeat;

	m_Sampler = Nexus::ResourcePool::Get()->GetSampler(samplerSpecs);

	m_TextureIDs.resize(Nexus::Module::Renderer::Get()->GetSwapchain()->GetImageCount());
}

void NexusEd::Viewport::SetContext(Nexus::Ref<Nexus::Framebuffer> framebuffer, uint32_t attachmentIndex)
{
	m_Framebuffer = framebuffer;
	m_Extent = Nexus::Module::Renderer::Get()->GetSwapchain()->GetExtent();

	for (uint32_t i = 0; i < m_TextureIDs.size(); i++)
	{
		ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)m_TextureIDs[i]);

		VkSampler samp = Nexus::DynamicPointerCast<Nexus::VulkanSampler>(m_Sampler)->Get();
		VkImageView view = Nexus::DynamicPointerCast<Nexus::VulkanFramebuffer>(m_Framebuffer)->GetAttachmentView(i, attachmentIndex);

		m_TextureIDs[i]  = (ImTextureID)ImGui_ImplVulkan_AddTexture(samp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

void NexusEd::Viewport::Render()
{
	uint32_t frameIndex = Context::s_Instance->GetFrameIndex();
	Context::s_Instance->BindTextureId(m_TextureIDs[frameIndex]);

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
