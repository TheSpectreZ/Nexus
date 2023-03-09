#include "EditorLayer.h"

void EditorLayer::OnAttach()
{
	NEXUS_LOG_DEBUG("Editor Layer Attached");

	Nexus::Ref<Nexus::Shader> SimpleShader = Nexus::Shader::Create("shaders/simple.shader");

	{
		Nexus::PipelineCreateInfo Info{};
		Info.shader = SimpleShader;

		m_Pipeline = Nexus::Pipeline::Create(Info);
	}

	{
		Nexus::Extent Extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		m_viewport.x = 0.0f;
		m_viewport.y = 0.0f;
		m_viewport.width = (float)Extent.width;
		m_viewport.height = (float)Extent.height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;

		m_scissor.Offset = { 0,0 };
		m_scissor.Extent = { Extent.width, Extent.height };
	}
}

void EditorLayer::OnUpdate()
{
	Nexus::RenderCommand::BindPipeline(m_Pipeline);

	Nexus::RenderCommand::SetViewport(m_viewport);
	Nexus::RenderCommand::SetScissor(m_scissor);

	Nexus::RenderCommand::Draw(3, 1, 0, 0);
}

void EditorLayer::OnDetach()
{
	m_Pipeline->~Pipeline();

	NEXUS_LOG_DEBUG("Editor Layer Detached");
}
