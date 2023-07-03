#include "NxRenderer/Renderer.h"
#include "NxRenderer/ResourcePool.h"

Nexus::Module::Renderer* Nexus::Module::Renderer::s_Instance = nullptr;

void Nexus::Module::Renderer::Initialize(const RendererCreateInfo& Info)
{
	s_Instance = new Renderer;
	
	s_Instance->m_Context = GraphicsInterface::CreateContext(Info.apiType, { Info.window->nativeHandle,Info.HInstance });
	s_Instance->m_Context->Initialize();

	s_Instance->m_Swapchain = GraphicsInterface::CreateSwapchain(Info.window);
	s_Instance->m_Swapchain->Initialize();

	s_Instance->m_CommandQueue = GraphicsInterface::CreateCommandQueue(Info.resizeCallback);
	s_Instance->m_CommandQueue->Initialize();

	ResourcePool::Initialize();
}

void Nexus::Module::Renderer::Shutdown()
{	
	ResourcePool::Shutdown();

	s_Instance->m_CommandQueue->Shutdown();
	s_Instance->m_CommandQueue.reset();

	s_Instance->m_Swapchain->Shutdown();
	s_Instance->m_Swapchain.reset();

	s_Instance->m_Context->Shutdown();
	s_Instance->m_Context.reset();

	delete s_Instance;
}

void Nexus::Module::Renderer::Begin()
{
	m_CommandQueue->BeginRenderQueue();
}

void Nexus::Module::Renderer::End()
{
	m_CommandQueue->EndRenderQueue();
}

void Nexus::Module::Renderer::FlushRender()
{
	m_CommandQueue->FlushRenderQueue();
}

void Nexus::Module::Renderer::FlushTransfer()
{
	m_CommandQueue->FlushTransferQueue();
}