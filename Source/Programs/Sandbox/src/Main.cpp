#ifdef NEXUS_DIST
#define WINDOWED
#endif // NEXUS_DIST

#include "Application/EntryPoint.h"
#include "SandboxLayer.h"

class LauncherApp : public Nexus::Application
{
public:
	LauncherApp()
	{
		p_Window.title = "Sandbox";
		p_Window.width = 800;
		p_Window.height = 600;
		p_Window.handle = nullptr;

		PushLayer<SandboxLayer>();
	}
};

Nexus::Application* CreateApplication()
{
	return new LauncherApp();
}