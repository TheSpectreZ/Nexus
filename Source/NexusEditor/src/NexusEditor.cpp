#include "EntryPoint.h"
#include "EditorLayer.h"

class NexusEditor : public Nexus::Application
{
public:
	NexusEditor()
	{
		m_AppSpecs.Window_Width = 1280;
		m_AppSpecs.Window_height = 720;
		m_AppSpecs.Window_Title = "Nexus Editor";
		m_AppSpecs.rApi = Nexus::RenderAPI_Vulkan;
		m_AppSpecs.pApi = Nexus::PhysicsAPI_Jolt;
		m_AppSpecs.Vsync = true;
		m_AppSpecs.MultiSampling = true;

		PushLayer(new EditorLayer());
	}
};

Nexus::Application* CreateApplication()
{
	return new NexusEditor();
}