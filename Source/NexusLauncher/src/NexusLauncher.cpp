#include "EntryPoint.h"
#include "LauncherLayer.h"

class Launcher : public Nexus::Application
{
public:
	Launcher()
	{
		m_AppSpecs.Window_Title = "Nexus Launcher";
		m_AppSpecs.Window_Width = 800;
		m_AppSpecs.Window_height = 600;
		m_AppSpecs.rApi = Nexus::RenderAPI_Vulkan;
		m_AppSpecs.pApi = Nexus::PhysicsAPI_None;
		m_AppSpecs.MultiSampling = true;
		m_AppSpecs.Vsync = true;
		m_AppSpecs.EnableScriptEngine = false;
		m_AppSpecs.LoadDefaultAssets = false;

		PushLayer(new LauncherLayer());
	}
};

Nexus::Application* CreateApplication(int argc, char** argv)
{
	return new Launcher();
}
