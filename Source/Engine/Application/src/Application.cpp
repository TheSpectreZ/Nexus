#include "Application/Application.h"
#include "DebugUtils/Logger.h"

#include "Platform/Manager.h"
#include "Platform/Input.h"

#include "Graphics/Engine.h"

#include "UserInterface/UserInterface.h"
#include "UserInterface/Manager.h"

#include <algorithm>

Nexus::Application::Application()
{
	p_Window.title = "Nexus Application";
	p_Window.handle = nullptr;
	p_Window.width = 1080;
	p_Window.height = 720;
}

void Nexus::Application::Run()
{
	NEXUS_LOG_INIT;
	NEXUS_LOG_TRACE("Nexus Logger Initialized")

	Platform::Initialize();
	Platform::Manager::Create_Window(p_Window);
	Platform::Input::SetContextWindow(p_Window);

	Graphics::EngineSpecification Specs{ &p_Window };
	Graphics::Engine::Get().Initialize(Specs, NX_BIND_EVENT_FN(Application::OnWindowResize));
	
	UserInterface::Initialize();
	UserInterface::Manager::Get()->InitWithVulkan(&p_Window);

	for (auto& l : m_layerstack)
		l->OnAttach();

	while (Platform::Manager::IsOpen(p_Window))
	{
		Platform::Manager::PollEvents();

		for (auto& l : m_layerstack)
			l->OnUpdate();

		auto cmd = Graphics::Engine::Get().StartFrame();

		for (auto& l : m_layerstack)
			l->OnRender();

		UserInterface::Manager::Get()->StartVulkanUIFrame();
		for (auto& l : m_layerstack)
			l->OnUIRender();
		UserInterface::Manager::Get()->EndVulkanUIFrame();

		UserInterface::Manager::Get()->RenderVulkanUIFrame(cmd);

		Graphics::Engine::Get().EndFrame();
	}

	Graphics::Engine::Get().WaitForDevice();

	UserInterface::Manager::Get()->ShutWithVulkan();
	UserInterface::Shutdown();

	for (auto& l : m_layerstack)
	{
		l->OnDetach();
	}

	Graphics::Engine::Get().Shutdown();

	Platform::Manager::Destroy_Window(p_Window);
	Platform::Shutdown();

	NEXUS_LOG_SHUT
}

void Nexus::Application::OnWindowResize(uint32_t width,uint32_t height)
{
	UserInterface::Manager::Get()->OnWindowResize(width, height);

	for (auto& l : m_layerstack)
		l->OnWindowResize(width, height);
}
