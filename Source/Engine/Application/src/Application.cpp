#include "Application/Application.h"
#include "DebugUtils/Logger.h"

#include "Platform/Manager.h"
#include "Platform/Input.h"

#include "Graphics/Engine.h"
#include "Graphics/Presenter.h"

#include <algorithm>

void Nexus::Application::Run()
{
	NEXUS_LOG_INIT;
	NEXUS_LOG_TRACE("Nexus Logger Initialized")

	Platform::Initialize();
	Platform::Manager::Create_Window(&p_Window);
	Platform::Input::SetContextWindow(&p_Window);

	Graphics::EngineSpecification Specs{ &p_Window };
	Graphics::Engine::Initialize(Specs);

	Graphics::Presenter::WindowResizeCallbackFnc = NX_BIND_EVENT_FN(Application::OnWindowResize);

	for (auto& l : m_layerstack)
		l->OnAttach();

	while (Platform::Manager::IsOpen(&p_Window))
	{
		Platform::Manager::PollEvents();

		for (auto& l : m_layerstack)
			l->OnUpdate();

		for (auto& l : m_layerstack)
			l->OnRender();
	}

	Graphics::Presenter::WaitForDevice();

	for (auto& l : m_layerstack)
	{
		l->OnDetach();
	}

	Graphics::Engine::Shutdown();

	Platform::Manager::Destroy_Window(&p_Window);
	Platform::Shutdown();

	NEXUS_LOG_SHUT
}

void Nexus::Application::OnWindowResize(uint32_t width,uint32_t height)
{
	for (auto& l : m_layerstack)
		l->OnWindowResize(width, height);
}
