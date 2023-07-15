#define NOMINMAX
#include "NxApplication/EntryPoint.h"
#include "NxApplication/Application.h"

#include "AppLayer.h"

class NexusEditor : public Nexus::Application
{
public:
	NexusEditor()
	{
		m_AppSpecs.Window_Title = "NexusEd";
		m_AppSpecs.Window_Width = 1600;
		m_AppSpecs.Window_height = 900;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;

		PushLayer(new AppLayer);
	}
};

Nexus::Application* CreateApplication(std::string commandLine)
{
	return new NexusEditor;
}