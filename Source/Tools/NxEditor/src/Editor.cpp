#define NOMINMAX
#include "NxApplication/EntryPoint.h"
#include "NxApplication/Application.h"

#include "AppLayer.h"

class NexusEditor : public Nexus::Application
{
public:
	NexusEditor(const std::unordered_map<std::string, std::string>& ccMap)
	{
		m_AppSpecs.Window_Title = "Nexus Editor";
		m_AppSpecs.Window_Width = 1600;
		m_AppSpecs.Window_height = 900;
		m_AppSpecs.rApi = Nexus::RendererAPI::VULKAN;

		m_AppSpecs.EnableRendererSubmodules = true;
		m_AppSpecs.EnableScriptEngine = true;
		m_AppSpecs.EnableAssetManager = true;
		m_AppSpecs.EnablePhysicsEngine = true;

		PushLayer(new AppLayer(ccMap));
	}
};

Nexus::Application* CreateApplication(const std::unordered_map<std::string, std::string>& ccMap)
{
	return new NexusEditor(ccMap);
}