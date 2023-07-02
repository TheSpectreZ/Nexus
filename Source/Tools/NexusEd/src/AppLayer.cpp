#include "AppLayer.h"
#include "NxGraphics/TypeImpls.h"
#include "NxRenderer/Renderer.h"
#include "NxScene/Entity.h"
#include "NxCore/Input.h"
#include "NxAsset/Manager.h"

#include "ImGui/Context.h"
#include "imgui.h"

using namespace Nexus;

void AppLayer::OnAttach()
{
	NexusEd::Context::Initialize();

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

		auto result = Module::AssetManager::Get()->Load(AssetType::Mesh, "Projects\\Assets\\cube.NxAsset");

		RenderableMeshSpecification specs{};
		specs.Type = MeshType::Static;
		specs.meshSpecs = DynamicPointerCast<MeshAsset>(result.asset)->GetMeshSpecifications();

		Module::Renderer::Get()->GetResourcePool()->AllocateRenderableMesh(specs, result.id);

		{
			auto entity = m_EditorScene->CreateEntity();
			auto& MeshComponent = entity.AddComponent<Component::Mesh>();
			MeshComponent.handle = result.id;

		}
		
		{
			auto entity = m_EditorScene->CreateEntity();
			auto& TransformComponent = entity.GetComponent<Component::Transform>();
			TransformComponent.Translation = glm::vec3(5.f, 0.f, 0.f);

			auto& MeshComponent = entity.AddComponent<Component::Mesh>();
			MeshComponent.handle = result.id;
		}
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

	NexusEd::Context::Get()->BeginFrame();
	ImGui::ShowDemoWindow();
	NexusEd::Context::Get()->EndFrame();
}

void AppLayer::OnDetach()
{
	m_EditorScene->Clear();

	NexusEd::Context::Shutdown();
}

void AppLayer::OnWindowResize(int width, int height)
{
	NexusEd::Context::OnWindowResize();
	
	m_EditorCameraController.SetPerspectiveProjection(45.f, (float)width, (float)height, 0.1f, 1000.f);
}
