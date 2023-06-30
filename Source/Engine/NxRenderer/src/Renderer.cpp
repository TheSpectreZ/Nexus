#include "NxRenderer/Renderer.h"

Nexus::Module::Renderer* Nexus::Module::Renderer::s_Instance = nullptr;

void Nexus::Module::Renderer::Initialize(const RendererCreateInfo& Info)
{
	s_Instance = new Renderer;
	
	s_Instance->m_Context = GraphicsInterface::CreateContext(Info.apiType, { Info.window->hwnd,Info.HInstance });
	s_Instance->m_Context->Initialize();

	s_Instance->m_Swapchain = GraphicsInterface::CreateSwapchain(Info.window);
	s_Instance->m_Swapchain->Initialize();

	auto callback = [&]() -> decltype(auto) 
	{ 
		s_Instance->ResizeCallback();
		return Info.resizeCallback(); 
	};

	s_Instance->m_CommandQueue = GraphicsInterface::CreateCommandQueue(callback);
	s_Instance->m_CommandQueue->Initialize();

	s_Instance->m_ResourcePool = CreateRef<ResourcePool>();
	s_Instance->m_ResourcePool->Initialize();
	
	s_Instance->m_ForwardDrawer = CreateRef<ForwardDrawer>();
}

void Nexus::Module::Renderer::Shutdown()
{
	s_Instance->m_ForwardDrawer.reset();
	
	s_Instance->m_ResourcePool->Shutdown();
	s_Instance->m_ResourcePool.reset();

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

void Nexus::Module::Renderer::Submit(Ref<Scene> scene)
{
	m_ForwardDrawer->Draw(scene);
}

void Nexus::Module::Renderer::FlushTransfer()
{
	m_CommandQueue->FlushTransferQueue();
}

void Nexus::Module::Renderer::ResizeCallback()
{
	Extent extent = m_Swapchain->GetExtent();

	m_ForwardDrawer->OnWindowResize(extent);
}
