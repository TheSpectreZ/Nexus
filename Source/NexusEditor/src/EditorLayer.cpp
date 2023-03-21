#include "EditorLayer.h"

void EditorLayer::OnAttach()
{
	NEXUS_LOG_DEBUG("Editor Layer Attached");

	Nexus::Ref<Nexus::Shader> SimpleShader = Nexus::Shader::Create("shaders/simple.shader");

	// Pipeline
	{
		Nexus::PipelineCreateInfo Info{};
		Info.shader = SimpleShader;

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

	// Mesh
	{
		m_Mesh = Nexus::AssetManager::LoadFromFile<Nexus::StaticMesh>("res/Meshes/Suzane.fbx");
	}

}

void EditorLayer::OnUpdate()
{
	m_cameraController.Move();
}

void EditorLayer::OnRender()
{
	Nexus::Command::BindPipeline(m_Pipeline);

	Nexus::Command::SetPushConstantData(m_Pipeline, (void*)glm::value_ptr(m_camera.projection* m_camera.view), sizeof(glm::mat4));

	Nexus::Command::SetViewport(m_viewport);
	Nexus::Command::SetScissor(m_scissor);

	Nexus::Ref<Nexus::StaticMesh> mesh = Nexus::AssetManager::Get<Nexus::StaticMesh>(m_Mesh);
	Nexus::Command::DrawMesh(mesh);
}

void EditorLayer::OnDetach()
{
	m_Pipeline->~Pipeline();

	NEXUS_LOG_DEBUG("Editor Layer Detached");
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
