#include "Platform/Manager.h"

int main()
{
	Nexus::Platform::Initialize();

	Nexus::Platform::Window appWindow = { 800,600,nullptr };
	Nexus::Platform::Manager::CreateWindow(&appWindow, "Sandbox");

	while (Nexus::Platform::Manager::IsOpen(&appWindow))
	{
		Nexus::Platform::Manager::PollEvents();
	}

	Nexus::Platform::Manager::DestroyWindow(&appWindow);
	Nexus::Platform::Shutdown();
}