#include "AppLayer.h"
#include "NxGraphics/TypeImpls.h"
#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/BatchRenderer.h"	
#include "NxRenderEngine/ResourcePool.h"
#include "NxScene/Entity.h"
#include "NxCore/ProjectSerializer.h"
#include "NxCore/Input.h"
#include "NxCore/Logger.h"
#include "NxApplication/FileDialog.h"
#include "NxApplication/Application.h"
#include "NxScriptEngine/ScriptEngine.h"	
#include "NxPhysicsEngine/PhysicsEngine.h"
#include "NxImGui/Context.h"
#include "imgui.h"

using namespace Nexus;

AppLayer::AppLayer(std::string& projectPath)
{
	m_ViewportSize = { 0.f,0.f };

	if (projectPath.empty())
		m_ProjectPath = "Sandbox/Sandbox.NxProj";
	else
		m_ProjectPath = projectPath;

	m_IsScenePlaying = false;
	m_IsScenePaused = false;
}

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

		auto& root = m_EditorScene->GetRootEntity();
		root.environment.handle = UUID();
		EnvironmentBuilder::Build("Resources/Textures/pink_sunrise_2k.hdr", root.environment.handle);

		auto entity = m_EditorScene->CreateEntity("Mesh");
		entity.AddComponent<Nexus::Component::Mesh>();
		entity.AddComponent<Nexus::Component::Script>("Sandbox.TestPlayer");
	}

	// Editor
	{
		auto& window = Application::Get()->GetWindow();
		auto rAPI = Application::Get()->GetAppSpecs().rApi;

		NxImGui::Context::Initialize(window, rAPI);

		m_Viewport.Initialize();
		m_Viewport.SetContext(m_ForwardDrawer->GetFramebuffer(), m_ForwardDrawer->GetResolveIndex());

		m_ContentBrowser.Initialize();
		m_SceneHeirarchy.SetContext(m_EditorScene);
	}

	LoadProject(m_ProjectPath);
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

	if (m_IsScenePlaying && !m_IsScenePaused)
	{
		ScriptEngine::OnSceneUpdate(dt);
		PhysicsEngine::OnSceneUpdate(dt);
		
		if (m_DrawColliders)
			PhysicsEngine::DrawColliders();
	}
}

void AppLayer::OnRender()
{
	m_ForwardDrawer->Draw(m_IsScenePlaying ? m_RuntimeScene : m_EditorScene);

	// Editor
	{
		NxImGui::Context::StartFrame();

		m_ContentBrowser.Render();
		m_SceneHeirarchy.Render();

		m_Viewport.Render();

		RenderSettingPanel();
		RenderTopMenuBarPanel();

		NxImGui::Context::EndFrame();
	}
}

void AppLayer::OnDetach()
{
	m_EditorScene.reset();
	m_RuntimeScene.reset();
	m_ForwardDrawer.reset();

	// Editor
	{
		NxImGui::Context::Shutdown();
	}
}

void AppLayer::OnWindowResize(int width, int height)
{
	m_EditorCameraController.SetPerspectiveProjection(45.f, (float)width, (float)height, 0.1f, 1000.f);

	Extent extent = { (uint32_t)width,(uint32_t)height };

	NxImGui::Context::OnWindowResize(extent);

	m_ForwardDrawer->OnWindowResize(extent);
	m_Viewport.SetContext(m_ForwardDrawer->GetFramebuffer(), m_ForwardDrawer->GetResolveIndex());
}

void AppLayer::RenderSettingPanel()
{
	ImGui::Begin("Setting");

	static float speed = 5.f;

	if (ImGui::DragFloat("Camera Speed", &speed, 5.f, 100.f))
		m_EditorCameraController.SetSpeed(speed);

	ImGui::Checkbox("Visualize Physics Collider", &m_DrawColliders);

	ImGui::End();
}

void AppLayer::LoadProject(const std::string& path)
{
	if (Nexus::ProjectSerializer::DeSerialize(path, m_ProjectSpecs))
	{
		m_ContentBrowser.SetContext(m_ProjectSpecs.RootPath);

		auto ScriptDllPath = m_ProjectSpecs.RootPath + "/Scripts/Bin/";

#ifdef NEXUS_DEBUG
		ScriptDllPath += "Debug/";
#elif NEXUS_RELEASE
		ScriptDllPath += "Release/";
#elif NEXUS_DIST
		ScriptDllPath += "Dist/";
#endif // NEXUS_DEBUG

		ScriptDllPath += m_ProjectSpecs.Name + ".dll";
		ScriptEngine::SetAppAssemblyFilepath(ScriptDllPath);
	}
}

void AppLayer::RenderTopMenuBarPanel()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Project"))
			{
				std::string path = Nexus::FileDialog::OpenFile("Nexus Project (*.nxProject)\0*.nxProject\0");
				if (!path.empty())
				{
					LoadProject(path);
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::MenuItem("Play"))
			{
				if (!m_IsScenePlaying)
				{
					m_RuntimeScene = m_EditorScene->Duplicate();

					ScriptEngine::OnSceneStart(m_RuntimeScene);
					PhysicsEngine::OnSceneStart(m_RuntimeScene);
					m_IsScenePlaying = true;
					NEXUS_LOG("Editor", "Started Editor Scene");
				}
			}

			if (ImGui::MenuItem("Stop"))
			{
				if (m_IsScenePlaying)
				{
					m_RuntimeScene.reset();
					ScriptEngine::OnSceneStop();
					PhysicsEngine::OnSceneStop();
					m_IsScenePlaying = false;
					NEXUS_LOG("Editor", "Stopped Editor Scene");
				}
			}

			if (ImGui::MenuItem("Pause/Resume"))
			{
				m_IsScenePaused = !m_IsScenePaused;
				NEXUS_LOG("Editor", "Paused Editor Scene");
			}

			if (ImGui::MenuItem("Reload Scripts"))
				ScriptEngine::ReloadAssembly();
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
