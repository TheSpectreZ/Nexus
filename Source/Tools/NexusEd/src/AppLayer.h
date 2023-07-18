#pragma once
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
	void OnAttach() override;
	void OnUpdate(float dt) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
	~AppLayer() override = default;
private:
	Nexus::Ref<Nexus::ForwardDrawer> m_ForwardDrawer;
	ImTextureID m_ID;
	glm::vec2 m_ViewportSize;
	NexusEd::Viewport m_Viewport;
	NexusEd::ContentBrowser m_ContentBrowser;
	NexusEd::SceneHeirarchy m_SceneHeirarchy;

	Nexus::Camera m_EditorCamera;
	Nexus::CameraController m_EditorCameraController;
	Nexus::Ref<Nexus::Scene> m_EditorScene;
};
