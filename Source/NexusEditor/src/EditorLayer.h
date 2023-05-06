#pragma once
#include "Nexus.h"

class EditorLayer : public Nexus::Layer
{
public:
	void OnAttach() override;
	void OnUpdate(Nexus::Timestep step) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
private:
	void CreateRenderpassAndFramebuffers();

	void RenderEditorMainMenu();
private:
	bool m_IsScenePlaying;
  
	Nexus::Ref<Nexus::Renderpass> m_GraphicsPass;
	Nexus::FramebufferSpecification m_GraphicsFBspecs;
	Nexus::Ref<Nexus::Framebuffer> m_GraphicsFramebuffer;

	Nexus::Ref<Nexus::Renderpass> m_ImGuiPass;
	Nexus::FramebufferSpecification m_ImGuiFBspecs;
	Nexus::Ref<Nexus::Framebuffer> m_ImGuiFramebuffer;

	glm::vec2 m_ImGuiEditorViewportSize;
	Nexus::Ref<Nexus::EditorViewport> m_ImGuiEditorViewport;

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

