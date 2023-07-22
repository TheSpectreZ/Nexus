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
	Nexus::ProjectSpecifications m_projectSpecs;

	Nexus::Ref<Nexus::ForwardDrawer> m_ForwardDrawer;
	
	glm::vec2 m_ViewportSize;
	NexusEd::Viewport m_Viewport;
	NexusEd::ContentBrowser m_ContentBrowser;
	NexusEd::SceneHeirarchy m_SceneHeirarchy;

	Nexus::Camera m_EditorCamera;
	Nexus::CameraController m_EditorCameraController;
	Nexus::Ref<Nexus::Scene> m_EditorScene;

	void LoadProject(const std::string& path);

	void RenderTopMenuBarPanel();
	void RenderSettingPanel();
};
