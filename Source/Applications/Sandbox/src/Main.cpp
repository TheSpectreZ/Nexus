#include "Platform/Manager.h"
#include "Platform/Input.h"

int main()
{
	Nexus::Platform::Initialize();

	Nexus::Platform::Window appWindow = { 800,600,nullptr };
	Nexus::Platform::Manager::CreateWindow(&appWindow, "Sandbox");
	
	Nexus::Platform::Input::SetContextWindow(&appWindow);

	while (Nexus::Platform::Manager::IsOpen(&appWindow))
	{
		Nexus::Platform::Manager::PollEvents();

		if (Nexus::Platform::Input::IsKeyPressed(Key::Escape))
			Nexus::Platform::Manager::Close(&appWindow);

	}

	Nexus::Platform::Manager::DestroyWindow(&appWindow);
	Nexus::Platform::Shutdown();
}