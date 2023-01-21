#include "Platform/Manager.h"
#include "Platform/Input.h"
#include "DebugUtils/Logger.h"

#include "Graphics/Graphics.h"

int main()
{
	NEXUS_LOG_INIT
	NEXUS_LOG_TRACE("Logger Initialized")
	
	Nexus::Platform::Initialize();

	Nexus::Platform::Window appWindow = { 800,600,nullptr };
	Nexus::Platform::Manager::Create_Window(&appWindow, "Sandbox");
	
	Nexus::Platform::Input::SetContextWindow(&appWindow);

	Nexus::Graphics::EngineSpecification Specifications{ appWindow.handle };
	Nexus::Graphics::Engine::Initialize(Specifications);

	while (Nexus::Platform::Manager::IsOpen(&appWindow))
	{
		Nexus::Platform::Manager::PollEvents();

		Nexus::Graphics::Engine::Render();

		if (Nexus::Platform::Input::IsKeyPressed(Key::Escape))
			Nexus::Platform::Manager::Close(&appWindow);

	}

	Nexus::Graphics::Engine::Shutdown();

	Nexus::Platform::Manager::Destroy_Window(&appWindow);
	Nexus::Platform::Shutdown();

	Nexus::DebugUtils::Logger::Shut();

	NEXUS_LOG_SHUT
}