#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/ResourcePool.h"
#include "NxRenderEngine/EnvironmentBuilder.h"	

Nexus::Renderer* Nexus::Renderer::s_Instance = nullptr;

void Nexus::Renderer::Initialize(const RendererCreateInfo& Info)
{
	s_Instance = new Renderer;
	
	s_Instance->m_Context = GraphicsInterface::CreateContext(Info.apiType, { Info.window->nativeHandle,Info.HInstance });
	s_Instance->m_Context->Initialize();

	s_Instance->m_Swapchain = GraphicsInterface::CreateSwapchain(Info.window);
	s_Instance->m_Swapchain->Initialize();

	s_Instance->m_CommandQueue = GraphicsInterface::CreateCommandQueue(Info.resizeCallback);
	s_Instance->m_CommandQueue->Initialize();

	s_Instance->m_ShaderBank = CreateRef<ShaderBank>();

	ResourcePool::Initialize();

	// Shaders
	{
		s_Instance->m_ShaderBank->Load("Resources/Shaders/skybox.glsl");
		s_Instance->m_ShaderBank->Load("Resources/Shaders/pbr.glsl");
	}
}

void Nexus::Renderer::Shutdown()
{	
	ResourcePool::Shutdown();
	
	s_Instance->m_ShaderBank.reset();

	s_Instance->m_CommandQueue->Shutdown();
	s_Instance->m_CommandQueue.reset();

	s_Instance->m_Swapchain->Shutdown();
	s_Instance->m_Swapchain.reset();

	s_Instance->m_Context->Shutdown();
	s_Instance->m_Context.reset();

	delete s_Instance;
}

void Nexus::Renderer::Begin()
{
	s_Instance->m_CommandQueue->BeginRenderQueue();
}

void Nexus::Renderer::End()
{
	s_Instance->m_CommandQueue->EndRenderQueue();
}

void Nexus::Renderer::WaitForRenderer()
{
	s_Instance->m_Context->WaitForDevice();
}

void Nexus::Renderer::FlushRender()
{
	s_Instance->m_CommandQueue->FlushRenderQueue();
}

void Nexus::Renderer::FlushTransfer()
{
	s_Instance->m_CommandQueue->FlushTransferQueue();
}
