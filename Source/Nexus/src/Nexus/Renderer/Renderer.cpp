#include "nxpch.h"
#include "Renderer.h"

Nexus::Ref<Nexus::Renderer> Nexus::Renderer::s_Renderer;

void Nexus::Renderer::Init(const RendererSpecifications& specs)
{
	s_Renderer = CreateRef<Renderer>();

	RenderAPI::SetRenderAPI(specs.api);

	s_Renderer->m_Context = Context::Create();
	s_Renderer->m_Context->Init();

	s_Renderer->m_Swapchain = Swapchain::Create();
	s_Renderer->m_Swapchain->Init();

	s_Renderer->m_RenderCommandQueue = RenderCommandQueue::Create();
	RenderCommand::Init();
}

void Nexus::Renderer::Shut()
{
	s_Renderer->m_RenderCommandQueue.reset();
	RenderCommand::Shut();

	s_Renderer->m_Swapchain->Shut();
	s_Renderer->m_Swapchain.reset();

	s_Renderer->m_Context->Shut();
	s_Renderer->m_Context.reset();
}

void Nexus::Renderer::Begin()
{
	s_Renderer->m_RenderCommandQueue->Begin();
	s_Renderer->m_Swapchain->BeginRenderPass();

	RenderCommand::Update();
}

void Nexus::Renderer::End()
{
	s_Renderer->m_Swapchain->EndRenderPass();
	s_Renderer->m_RenderCommandQueue->End();
}

void Nexus::Renderer::Flush()
{
	s_Renderer->m_RenderCommandQueue->Flush();
}

void Nexus::Renderer::WaitForDevice()
{
	s_Renderer->m_Context->WaitForDevice();
}
