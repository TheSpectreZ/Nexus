#include "NxRenderer/Renderer.h"

Nexus::Module::Renderer* Nexus::Module::Renderer::s_Instance = nullptr;

void Nexus::Module::Renderer::Initialize(const RendererCreateInfo& Info)
{
	s_Instance = new Renderer;
	
	s_Instance->m_Context = GraphicsInterface::CreateContext(Info.apiType, { Info.window->hwnd,Info.HInstance });
	s_Instance->m_Context->Initialize();

	s_Instance->m_Swapchain = GraphicsInterface::CreateSwapchain(Info.window);
	s_Instance->m_Swapchain->Initialize();

	s_Instance->m_CommandQueue = GraphicsInterface::CreateCommandQueue(Info.resizeCallback);
	s_Instance->m_CommandQueue->Initialize();
}

void Nexus::Module::Renderer::Shutdown()
{
	s_Instance->m_CommandQueue->Shutdown();
	s_Instance->m_Swapchain->Shutdown();
	s_Instance->m_Context->Shutdown();

	delete s_Instance;
}
