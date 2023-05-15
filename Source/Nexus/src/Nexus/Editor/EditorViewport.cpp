#include "nxpch.h"
#include "EditorContext.h"
#include "EditorViewport.h"
#include "Renderer/Renderer.h"

void Nexus::EditorViewport::Initialize()
{
	m_Sampler = Sampler::Create(SamplerFilter::Linear, SamplerFilter::Linear,SamplerWrapMode::Repeat,SamplerWrapMode::Repeat,SamplerWrapMode::Repeat);
	m_TextureIDs.resize(Renderer::GetSwapchain()->GetImageCount());
}

void Nexus::EditorViewport::SetContext(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex)
{
	m_Framebuffer = framebuffer;
	m_Extent = Renderer::GetSwapchain()->GetExtent();

	for (uint32_t i = 0; i < m_TextureIDs.size(); i++)
	{
		EditorContext::s_Instance->DestroyTextureID(m_TextureIDs[i]);
		m_TextureIDs[i] = EditorContext::s_Instance->MakeTextureID(m_Framebuffer, m_Sampler, i, attachmentIndex);
	}
}

void Nexus::EditorViewport::Render()
{
	uint32_t frameIndex = Renderer::GetCommandQueue()->GetFrameIndex();

	EditorContext::s_Instance->BindTextureID(m_TextureIDs[frameIndex]);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	m_Panelsize = ImGui::GetContentRegionAvail();

	ImGui::Image(m_TextureIDs[frameIndex], m_Panelsize);

	ImGui::End();
	ImGui::PopStyleVar();
}

glm::vec2 Nexus::EditorViewport::GetViewportSize()
{
	return { m_Panelsize.x,m_Panelsize.y };
}
