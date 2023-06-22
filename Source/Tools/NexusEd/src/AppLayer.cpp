#include "AppLayer.h"
#include "NxGraphics/TypeImpls.h"
#include "NxRenderer/Renderer.h"
#include "NxScene/Entity.h"
#include "NxCore/Input.h"

using namespace Nexus;

void AppLayer::OnAttach()
{
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
		m_EditorScene = Scene::Create();
	}
}

void AppLayer::OnUpdate(float dt)
{
	m_EditorCameraController.Update(dt);
}

void AppLayer::OnRender()
{
	// Rendering
	Module::Renderer::Get()->Submit(m_EditorScene);
}

void AppLayer::OnDetach()
{
	m_EditorScene->Clear();
}

void AppLayer::OnWindowResize(int width, int height)
{
	m_EditorCameraController.SetPerspectiveProjection(45.f, (float)width, (float)height, 0.1f, 1000.f);
}
