#define NOMINMAX
#include "NxApplication/EntryPoint.h"
#include "NxApplication/Application.h"

#include "AppLayer.h"

class NexusEd : public Nexus::Application
{
public:
	NexusEd()
	{
		m_AppSpecs.Window_Title = "NexusEd";
		m_AppSpecs.Window_Width = 1024;
		m_AppSpecs.Window_height = 578;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;

		PushLayer(new AppLayer);
	}
};

Nexus::Application* CreateApplication(std::string commandLine)
{
	return new NexusEd;
}