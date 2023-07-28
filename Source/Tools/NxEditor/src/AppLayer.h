#pragma once
#include "NxCore/ProjectSpecification.h"
#include "NxApplication/Layer.h"
#include "NxScene/Scene.h"
#include "NxScene/Camera.h"
#include "NxRenderEngine/Drawer.h"

#include "ImGui/Viewport.h"	
#include "ImGui/ContentBrowser.h"
#include "ImGui/SceneHeirarchy.h"

class AppLayer : public Nexus::Layer
{
public:
	AppLayer(std::string& projectPath);
	void OnAttach() override;
	void OnUpdate(float dt) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
	~AppLayer() override = default;
private:
	std::string m_ProjectPath;
	Nexus::ProjectSpecifications m_ProjectSpecs;

	Nexus::Ref<Nexus::ForwardDrawer> m_ForwardDrawer;
	
	glm::vec2 m_ViewportSize;
	NexusEd::Viewport m_Viewport;
	NexusEd::ContentBrowser m_ContentBrowser;
	NexusEd::SceneHeirarchy m_SceneHeirarchy, m_TempSH;

	Nexus::Camera m_EditorCamera;
	Nexus::CameraController m_EditorCameraController;
	
	bool m_IsScenePlaying = false, m_IsScenePaused = false, m_DrawColliders = false;;
	Nexus::Ref<Nexus::Scene> m_EditorScene, m_RuntimeScene;

	void LoadProject(const std::string& path);

	void RenderTopMenuBarPanel();
	void RenderSettingPanel();
};
