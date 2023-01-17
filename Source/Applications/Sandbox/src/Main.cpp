#include "Platform/Manager.h"
#include "Platform/Input.h"
#include "DebugUtils/Logger.h"

int main()
{
	NEXUS_LOG_INIT
	NEXUS_LOG_TRACE("Logger Initialized")
	
	Nexus::Platform::Initialize();

	Nexus::Platform::Window appWindow = { 800,600,nullptr };
	Nexus::Platform::Manager::Create_Window(&appWindow, "Sandbox");
	
	Nexus::Platform::Input::SetContextWindow(&appWindow);

	while (Nexus::Platform::Manager::IsOpen(&appWindow))
	{
		Nexus::Platform::Manager::PollEvents();

		if (Nexus::Platform::Input::IsKeyPressed(Key::Escape))
			Nexus::Platform::Manager::Close(&appWindow);

	}

	Nexus::Platform::Manager::Destroy_Window(&appWindow);
	Nexus::Platform::Shutdown();

	Nexus::DebugUtils::Logger::Shut();

	NEXUS_LOG_SHUT
}