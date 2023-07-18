#include "AppLayer.h"
#include "NxGraphics/TypeImpls.h"
#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/ResourcePool.h"
#include "NxScene/Entity.h"
#include "NxCore/Input.h"

#include "ImGui/Context.h"
#include "imgui.h"

using namespace Nexus;

void AppLayer::OnAttach()
{
	m_ForwardDrawer = CreateRef<ForwardDrawer>(true);

	Extent extent = Module::Renderer::Get()->GetSwapchain()->GetExtent();
	// Camera
	{
		m_EditorCameraController.AttachCamera(&m_EditorCamera);
		m_EditorCameraController.SetKeyBinding(CameraBinding::FRONT, Key::W);
		m_EditorCameraController.SetKeyBinding(CameraBinding::BACK, Key::S);
		m_EditorCameraController.SetKeyBinding(CameraBinding::LEFT, Key::A);
		m_EditorCameraController.SetKeyBinding(CameraBinding::RIGHT, Key::D);
		m_EditorCameraController.SetKeyBinding(CameraBinding::UP, Key::E);
		m_EditorCameraController.SetKeyBinding(CameraBinding::DOWN, Key::Q);
		m_EditorCameraController.SetKeyBinding(CameraBinding::MOUSE_UNLOCK, Mouse::Right);
		m_EditorCameraController.SetSpeed(5.f);
		m_EditorCameraController.SetPerspectiveProjection(45.f, (float)extent.width, (float)extent.height, 0.1f, 1000.f);
	}

	// Scene
	{
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->SetCamera(&m_EditorCamera);

		auto light = m_EditorScene->CreateEntity("Light");
		light.AddComponent<Component::DirectionalLight>();

		auto entity = m_EditorScene->CreateEntity("Mesh");

		auto& root = m_EditorScene->GetRootEntity();
		root.environment.handle = UUID();
		EnvironmentBuilder::Build("Resources/Textures/pink_sunrise_2k.hdr", root.environment.handle);
	}

	// Editor
	{
		NexusEd::Context::Initialize();

		m_Viewport.Initialize();
		m_Viewport.SetContext(m_ForwardDrawer->GetFramebuffer(), m_ForwardDrawer->GetResolveIndex());

		m_ContentBrowser.Initialize();
		m_ContentBrowser.SetContext("Projects");

		m_SceneHeirarchy.SetContext(m_EditorScene);

		m_ID = NexusEd::Context::Get()->CreateTextureId(EnvironmentBuilder::GetBRDFLut(), ResourcePool::Get()->GetSampler(11122));
	}
}

void AppLayer::OnUpdate(float dt)
{
	glm::vec2 size = m_Viewport.GetViewportSize();
	if (size != m_ViewportSize)
	{
		m_ViewportSize = size;
		if (size.x != 0 && size.y != 0)
			m_EditorCameraController.SetPerspectiveProjection(45.f, size.x, size.y, 0.1f, 1000.f);
	}

	m_EditorCameraController.Update(dt);
}

void AppLayer::OnRender()
{
	// Game
	m_ForwardDrawer->Draw(m_EditorScene);

	// Editor
	{
		NexusEd::Context::Get()->BeginFrame();

		m_ContentBrowser.Render();
		m_SceneHeirarchy.Render();
		m_Viewport.Render();

		ImGui::Begin("Setting");

		static float speed = 5.f;
		if (ImGui::DragFloat("Camera Speed", &speed, 5.f, 100.f))
			m_EditorCameraController.SetSpeed(speed);

		NexusEd::Context::Get()->BindTextureId(m_ID);
		ImGui::Image(m_ID, ImVec2(256, 256));

		ImGui::End();

		NexusEd::Context::Get()->EndFrame();
	}
}

void AppLayer::OnDetach()
{
	m_EditorScene->Clear();
	m_ForwardDrawer.reset();

	// Editor
	{
		NexusEd::Context::Shutdown();
	}
}

void AppLayer::OnWindowResize(int width, int height)
{
	m_EditorCameraController.SetPerspectiveProjection(45.f, (float)width, (float)height, 0.1f, 1000.f);

	Extent extent = { (uint32_t)width,(uint32_t)height };

	NexusEd::Context::OnWindowResize(extent);
	m_ForwardDrawer->OnWindowResize(extent);
	m_Viewport.SetContext(m_ForwardDrawer->GetFramebuffer(), m_ForwardDrawer->GetResolveIndex());
}
