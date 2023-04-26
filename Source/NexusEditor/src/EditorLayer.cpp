#include "EditorLayer.h"

void EditorLayer::OnAttach()
{
	NEXUS_LOG_DEBUG("Editor Layer Attached");

	Nexus::Ref<Nexus::Shader> simpleShader = Nexus::ShaderLib::Get("shaders/simple.shader");
	//Nexus::Ref<Nexus::Shader> pbrShader = Nexus::ShaderLib::Get("shaders/pbr.shader");
	
	// Pipeline
	{
		Nexus::PipelineCreateInfo Info{};
		Info.shader = simpleShader;

		Info.vertexBindInfo = Nexus::StaticMeshVertex::GetBindings();
		Info.vertexAttribInfo = Nexus::StaticMeshVertex::GetAttributes();

		Info.pushConstantInfo.resize(1);
		Info.pushConstantInfo[0].offset = 0;
		Info.pushConstantInfo[0].size = sizeof(glm::mat4);
		Info.pushConstantInfo[0].stage = Nexus::ShaderStage::Vertex;

		Info.rasterizerInfo.lineWidth = 1.f;
		Info.rasterizerInfo.frontFace = Nexus::FrontFaceType::Clockwise;
		Info.rasterizerInfo.cullMode = Nexus::CullMode::Back;
		Info.rasterizerInfo.polygonMode = Nexus::PolygonMode::Fill;

		m_Pipeline = Nexus::Pipeline::Create(Info);
	}

	// Screen
	{
		Nexus::Extent Extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		m_viewport.x = 0.0f;
		m_viewport.y = 0.0f;
		m_viewport.width = (float)Extent.width;
		m_viewport.height = (float)Extent.height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;

		m_scissor.Offset = { 0,0 };
		m_scissor.Extent = { Extent.width, Extent.height };
	}

	// Camera
	{
		using namespace Nexus;

		m_cameraController.AttachCamera(&m_camera);
		
		m_cameraController.SetKeyBindings(CameraBindings::FRONT, Key::W);
		m_cameraController.SetKeyBindings(CameraBindings::BACK, Key::S);
		m_cameraController.SetKeyBindings(CameraBindings::DOWN, Key::E);
		m_cameraController.SetKeyBindings(CameraBindings::UP, Key::Q);
		m_cameraController.SetKeyBindings(CameraBindings::LEFT, Key::A);
		m_cameraController.SetKeyBindings(CameraBindings::RIGHT, Key::D);

		Nexus::Extent extent = Renderer::GetSwapchain()->GetExtent();
		
		m_cameraController.SetPerspectiveProjection(45.f, (float)extent.width,(float)extent.height, 0.1f, 1000.f);
	}

	// Scene
	{
		Nexus::AssetHandle handle = Nexus::AssetManager::LoadFromFile<Nexus::StaticMeshAsset>("res/Meshes/Suzane.fbx");

		m_Scene = Nexus::Scene::Create();
		
		Nexus::Entity entity = m_Scene->CreateEntity();
		entity.AddComponent<Nexus::Component::Mesh>(handle);
		entity.AddComponent<Nexus::Component::Script>("Sandbox.Player");

		m_SceneData = Nexus::SceneBuildData::Build(m_Scene, simpleShader);
		
		m_SceneRenderer.SetContext(m_Scene, m_SceneData);
	}
}

void EditorLayer::OnUpdate(Nexus::Timestep ts)
{
	m_cameraController.Move();
	
	m_SceneData->Update(m_Scene, m_camera);

	if (m_IsScenePlaying)
	{
		Nexus::ScriptEngine::OnSceneStart(m_Scene);
	}
}

void EditorLayer::OnRender()
{
	Nexus::Command::BindPipeline(m_Pipeline);

	Nexus::Command::SetViewport(m_viewport);
	Nexus::Command::SetScissor(m_scissor);

	m_SceneRenderer.Render();
}

void EditorLayer::OnDetach()
{
	m_SceneData->Destroy();
	m_Scene->Clear();

	m_Pipeline->~Pipeline();

	NEXUS_LOG_DEBUG("Editor Layer Detached");
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("Script");
	ImGui::Text("Hellooo Scripting !");

	if (ImGui::Button("Start Scene"))
	{
		m_IsScenePlaying = true;
		Nexus::ScriptEngine::OnSceneStart(m_Scene);
	}

	if (ImGui::Button("End Scene") && m_IsScenePlaying)
	{
		m_IsScenePlaying = false;
		Nexus::ScriptEngine::OnSceneStop();
	}

	ImGui::End();
}

void EditorLayer::OnWindowResize(int width, int height)
{
	// Screen
	{
		Nexus::Extent Extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		m_viewport.x = 0.0f;
		m_viewport.y = 0.0f;
		m_viewport.width = (float)Extent.width;
		m_viewport.height = (float)Extent.height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;

		m_scissor.Offset = { 0,0 };
		m_scissor.Extent = { Extent.width, Extent.height };

		m_cameraController.SetPerspectiveProjection(45.f, (float)width, (float)height, 0.1f, 1000.f);
	}
}
