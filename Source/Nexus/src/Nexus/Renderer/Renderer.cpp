#include "nxpch.h"
#include "Renderer.h"
#include "Shader.h"

Nexus::Ref<Nexus::Renderer> Nexus::Renderer::s_Renderer;
std::function<void()> Nexus::Renderer::ResizeCallback;

void Nexus::Renderer::Init(const RendererSpecifications& specs)
{
	s_Renderer = CreateRef<Renderer>();

	RenderAPI::SetRenderAPI(specs.api);

	s_Renderer->m_Context = Context::Create();
	s_Renderer->m_Context->Init();

	s_Renderer->m_Swapchain = Swapchain::Create();
	s_Renderer->m_Swapchain->Init();

	s_Renderer->m_RenderCommandQueue = RenderCommandQueue::Create();
	s_Renderer->m_TransferCommandQueue = TransferCommandQueue::Create();

	Command::Init();

	ShaderLib::Initialize();
}

void Nexus::Renderer::Shut()
{
	ShaderLib::Terminate();

	Command::Shut();

	s_Renderer->m_RenderCommandQueue.reset();
	s_Renderer->m_TransferCommandQueue.reset();

	s_Renderer->m_Swapchain->Shut();
	s_Renderer->m_Swapchain.reset();

	s_Renderer->m_Context->Shut();
	s_Renderer->m_Context.reset();
}

void Nexus::Renderer::BeginRenderCommandQueue()
{
	s_Renderer->m_RenderCommandQueue->Begin();
	Command::Update();
}

void Nexus::Renderer::EndRenderCommandQueue()
{
	s_Renderer->m_RenderCommandQueue->End();
}

void Nexus::Renderer::FlushRenderCommandQueue()
{
	s_Renderer->m_RenderCommandQueue->Flush();
}

void Nexus::Renderer::FlushTransferCommandQueue()
{
	s_Renderer->m_TransferCommandQueue->Flush();
}

void Nexus::Renderer::WaitForDevice()
{
	s_Renderer->m_Context->WaitForDevice();
}
