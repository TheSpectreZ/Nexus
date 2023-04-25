#pragma once
#include "Nexus.h"

class EditorLayer : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate(Nexus::Timestep step) override;
	void OnRender() override;
	void OnDetach() override;
	void OnImGuiRender() override;
	void OnWindowResize(int width, int height) override;
private:
	bool m_IsScenePlaying;
	std::unordered_map<std::string, Nexus::ScriptInstance> m_ScriptInstance;

	Nexus::Ref<Nexus::Pipeline> m_Pipeline;
	Nexus::Ref<Nexus::Scene> m_Scene;
	
	Nexus::SceneRenderer::PhysicallyBased m_PBRsceneRenderer;

	Nexus::Viewport m_viewport;
	Nexus::Scissor m_scissor;

	Nexus::Camera m_camera;
	Nexus::CameraController m_cameraController;
};

