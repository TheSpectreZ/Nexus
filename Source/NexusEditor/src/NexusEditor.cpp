#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "EntryPoint.h"
#include "EditorLayer.h"

class NexusEditor : public Nexus::Application
{
public:
	NexusEditor(const std::string& projectPath)
	{
		m_AppSpecs.Window_Title = "Nexus Editor";
		m_AppSpecs.Window_Width = 1600;
		m_AppSpecs.Window_height = 900;
		m_AppSpecs.rApi = Nexus::RenderAPI_Vulkan;
		m_AppSpecs.pApi = Nexus::PhysicsAPI_Jolt;
		m_AppSpecs.MultiSampling = true;
		m_AppSpecs.Vsync = true;
		m_AppSpecs.EnableScriptEngine = true;
		m_AppSpecs.LoadDefaultAssets = true;

		PushLayer(new EditorLayer(projectPath));
	}
};

Nexus::Application* CreateApplication(std::string projectPath)
{
	return new NexusEditor(projectPath);
}