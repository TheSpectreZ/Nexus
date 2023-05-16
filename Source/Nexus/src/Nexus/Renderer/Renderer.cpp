#include "nxpch.h"
#include "Renderer.h"
#include "Shader.h"

Nexus::Ref<Nexus::Renderer> Nexus::Renderer::s_Renderer;
std::function<void()> Nexus::Renderer::ResizeCallback;

void Nexus::Renderer::BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer)
{
	s_Renderer->m_CommandQueue->BeginRenderPass(pass, framebuffer);
}

void Nexus::Renderer::EndRenderPass()
{
	s_Renderer->m_CommandQueue->EndRenderPass();
}

void Nexus::Renderer::BindPipeline(Ref<Pipeline> pipeline)
{
	s_Renderer->m_CommandQueue->BindPipeline(pipeline);
}

void Nexus::Renderer::SetScissor(Scissor scissor)
{
	s_Renderer->m_CommandQueue->SetScissor(scissor);
}

void Nexus::Renderer::SetViewport(Viewport viewport)
{
	s_Renderer->m_CommandQueue->SetViewport(viewport);
}

void Nexus::Renderer::DrawMesh(Ref<StaticMesh> mesh)
{
	s_Renderer->m_CommandQueue->DrawMesh(mesh);
}

void Nexus::Renderer::DrawSubMesh(SubMesh* submesh)
{
	s_Renderer->m_CommandQueue->DrawSubMesh(submesh);
}

void Nexus::Renderer::Init(const RendererSpecifications& specs)
{
	s_Renderer = CreateRef<Renderer>();

	RenderAPI::SetRenderAPI(specs.api);

	s_Renderer->m_Context = Context::Create();
	s_Renderer->m_Context->Init();

	s_Renderer->m_Swapchain = Swapchain::Create();
	s_Renderer->m_Swapchain->Init();

	s_Renderer->m_CommandQueue = CommandQueue::Create();
	s_Renderer->m_CommandQueue->Init();

	ShaderLib::Initialize();
}

void Nexus::Renderer::Shut()
{
	ShaderLib::Terminate();

	s_Renderer->m_CommandQueue->Shut();
	s_Renderer->m_CommandQueue.reset();

	s_Renderer->m_Swapchain->Shut();
	s_Renderer->m_Swapchain.reset();

	s_Renderer->m_Context->Shut();
	s_Renderer->m_Context.reset();
}

void Nexus::Renderer::BeginRenderCommandQueue()
{
	s_Renderer->m_CommandQueue->BeginRenderQueue();
}

void Nexus::Renderer::EndRenderCommandQueue()
{
	s_Renderer->m_CommandQueue->EndRenderQueue();
}

void Nexus::Renderer::FlushRenderCommandQueue()
{
	s_Renderer->m_CommandQueue->FlushRenderQueue();
}

void Nexus::Renderer::FlushTransferCommandQueue()
{
	s_Renderer->m_CommandQueue->FlushTransferQueue();
}

void Nexus::Renderer::WaitForDevice()
{
	s_Renderer->m_Context->WaitForDevice();
}
