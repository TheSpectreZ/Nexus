#include "Application/EntryPoint.h"

class LauncherApp : public Nexus::Application
{
public:
	LauncherApp()
	{
		p_Window.title = "Nexus Launcher";
		p_Window.width = 800;
		p_Window.height = 600;
		p_Window.handle = nullptr;
	}
};

Nexus::Application* CreateApplication()
{
	return new LauncherApp();
}