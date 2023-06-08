#include "EntryPoint.h"
#include "Application.h"

class NexusEd : public Nexus::Application
{
public:
	NexusEd()
	{
		m_AppSpecs.Window_Title = "NexusEd";
		m_AppSpecs.Window_Width = 1600;
		m_AppSpecs.Window_height = 900;
	}
};

Nexus::Application* CreateApplication(std::string commandLine)
{
	return new NexusEd;
}