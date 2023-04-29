#pragma once
#include "Nexus.h"

class EditorLayer : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDetach() override;
	void OnImGuiRender() override;
	void OnWindowResize(int width, int height) override;
private:
	Nexus::Ref<Nexus::Pipeline> m_Pipeline;
	
	Nexus::Ref<Nexus::Scene> m_Scene;
	Nexus::Ref<Nexus::SceneBuildData> m_SceneData;
	
	Nexus::SceneRenderer m_SceneRenderer;

	Nexus::SceneHeirarchy m_SceneHeirarchy;

	Nexus::Viewport m_viewport;
	Nexus::Scissor m_scissor;

	Nexus::Camera m_camera;
	Nexus::CameraController m_cameraController;
};

