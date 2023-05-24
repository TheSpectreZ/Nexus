#pragma once
#include "Nexus.h"

class EditorLayer : public Nexus::Layer
{
public:
	EditorLayer(const std::string& projectPath)
		:m_ProjectPath(projectPath), m_IsScenePlaying(false), m_PauseScene(false)
	{
		// Load Sandbox project If Path is Empty
	}

	void OnAttach() override;
	void OnUpdate(Nexus::Timestep step) override;
	void OnRender() override;
	void OnDetach() override;
	void OnWindowResize(int width, int height) override;
private:
	void CreateRenderpassAndFramebuffers();

	void LoadProject();

	void RenderProfileStats();
	void RenderEditorMainMenu();
	void RenderEditorWorldControls();
private:
	std::string m_ProjectPath;
	std::string m_ScriptDLLPath;
	Nexus::ProjectSpecifications m_ProjectSpecs;

	bool m_IsScenePlaying, m_PauseScene;
  
	Nexus::Ref<Nexus::Renderpass> m_GraphicsPass;
	Nexus::FramebufferSpecification m_GraphicsFBspecs;
	Nexus::Ref<Nexus::Framebuffer> m_GraphicsFramebuffer;

	Nexus::Ref<Nexus::Renderpass> m_ImGuiPass;
	Nexus::FramebufferSpecification m_ImGuiFBspecs;
	Nexus::Ref<Nexus::Framebuffer> m_ImGuiFramebuffer;

	glm::vec2 m_ImGuiEditorViewportSize;
	Nexus::EditorViewport m_ImGuiEditorViewport;
	Nexus::SceneHeirarchy m_SceneHeirarchy;
	Nexus::ContentBrowser m_ContentBrowser;

	Nexus::Ref<Nexus::Pipeline> m_Pipeline;
	
	Nexus::Ref<Nexus::Scene> m_EditorScene;
	Nexus::Ref<Nexus::Scene> m_RuntimeScene;
	Nexus::Ref<Nexus::Scene> m_CurrentScene;

	Nexus::Ref<Nexus::SceneBuildData> m_SceneData;
	
	Nexus::Ref<Nexus::PhysicsWorld> m_PhysicsWorld;

	Nexus::SceneRenderer m_SceneRenderer;

	Nexus::Viewport m_viewport;
	Nexus::Scissor m_scissor;

	Nexus::Camera m_camera;
	Nexus::CameraController m_cameraController;
};

