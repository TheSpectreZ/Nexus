#define NOMINMAX
#include "NxApplication/EntryPoint.h"
#include "NxApplication/Application.h"

#include "AppLayer.h"

class NexusEditor : public Nexus::Application
{
public:
	NexusEditor(std::string& commandLine)
	{
		m_AppSpecs.Window_Title = "Nexus Editor";
		m_AppSpecs.Window_Width = 1600;
		m_AppSpecs.Window_height = 900;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;
		m_AppSpecs.EnableRendererSubmodules = true;

		PushLayer(new AppLayer(commandLine));
	}
};

Nexus::Application* CreateApplication(std::string commandLine)
{
	return new NexusEditor(commandLine);
}