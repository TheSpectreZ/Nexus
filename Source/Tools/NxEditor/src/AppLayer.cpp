#include "AppLayer.h"

#include "NxGraphics/TypeImpls.h"

#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/BatchRenderer.h"	
#include "NxRenderEngine/ResourcePool.h"

#include "NxScene/Entity.h"

#include "NxCore/ProjectSerializer.h"
#include "NxCore/Registry.h"
#include "NxCore/Input.h"
#include "NxCore/Logger.h"

#include "NxApplication/FileDialog.h"
#include "NxApplication/Application.h"

#include "NxScriptEngine/ScriptEngine.h"	

#include "NxPhysicsEngine/PhysicsEngine.h"

#include "NxAsset/Importer.h"
#include "NxAsset/Manager.h"

#include "NxImGui/Context.h"
#include "imgui.h"

using namespace Nexus;

// This func is copied from SceneHeirarchy.cpp for Test Purposes
static void LoadMesh(const AssetFilePath& filepath, Component::Mesh& component, bool loadMat = true)
{
	if (filepath.extension().string() != ".NxMeshAsset")
		return;

	Nexus::MeshAssetSpecification specs{};
	Loader::LoadMeshAsset(filepath, &specs);

	AssetRegistry* reg = AssetRegistry::Get();
	{
		auto path = reg->LookUp(specs.mesh);
		Meshing::MeshSource mSrc;
		mSrc.Deserialize(path);

		RenderableMeshSpecification mSpecs{};
		mSpecs.source = mSrc;
		mSpecs.path = path;

		if (specs.skeleton)
		{
			path = reg->LookUp(specs.skeleton);
			mSpecs.skeleton.Deserialize(path);

			component.skeleton = specs.skeleton;
		}

		AssetManager::Get()->Load<RenderableMesh>(specs.mesh, mSpecs);

		component.handle = specs.mesh;
	}

	for (auto& [k, v] : specs.materials)
	{
		auto path = reg->LookUp(v);
		Meshing::Material mat;
		mat.Deserialize(path);

		RenderableMaterialSpecification mSpecs{};
		mSpecs.material = mat;
		mSpecs.path = path;

		AssetManager::Get()->Load<RenderableMaterial>(v, mSpecs);

		component.materialTable[k] = v;
	}
}

AppLayer::AppLayer(const std::unordered_map<std::string, std::string>& ccMap)
{
	m_ViewportSize = { 0.f,0.f };

	if (!ccMap.contains("ProjectPath"))
		m_ProjectPath = "Sandbox/Sandbox.NxProj";
	else
		m_ProjectPath = ccMap.at("ProjectPath");

	m_IsScenePlaying = false;
	m_IsScenePaused = false;
}

void AppLayer::OnAttach()
{
	SetupRenderGraph();

	m_ForwardDrawer = CreateRef<ForwardDrawer>(true);

	AssetRegistry::Get()->SetProjectContext(AssetFilePath(m_ProjectPath).parent_path());

	// Load and Allocate default Assets [ TODO: Do this in Render Engine ? ]
	{
		AssetFilePath path = "Resources/Meshes/Cube.NxMeshAsset";
		MeshAssetSpecification specs{};
		
		if (Loader::LoadMeshAsset(path, &specs))
		{
			auto fpath = AssetRegistry::Get()->LookUp(specs.mesh);

			RenderableMeshSpecification mSpecs{};
			mSpecs.path = fpath;
			mSpecs.source.Deserialize(fpath);

			RenderableMesh::AddToPool(specs.mesh, mSpecs);
		}

		{
			path = "Resources/Textures/white.NxTex";
			RenderableTextureSpecification tSpecs{};
			tSpecs.path = path;
			tSpecs.texture.Deserialize(path);

			auto tId = AssetRegistry::Get()->LookUp(path);

			RenderableTexture::AddToPool(tId, tSpecs);
		}

		{
			path = "Resources/Material/Default.NxMat";
			RenderableMaterialSpecification mSpecs{};
			mSpecs.path = path;
			mSpecs.material.Deserialize(path);

			auto mId = AssetRegistry::Get()->LookUp(path);

			RenderableMaterial::AddToPool(mId, mSpecs);
		}
	}
	
	// Camera
	{
		Extent extent = Renderer::GetSwapchain()->GetExtent();
		
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
		EnvironmentBuilder::Initialize();
		EnvironmentBuilder::Build("Resources/Textures/pink_sunrise_2k.hdr", root.environment.handle);

		// Default Test Scene...

		m_EditorCamera.position = glm::vec3(2.f, 3.f, 8.f);

		{
			auto Entity = m_EditorScene->CreateEntity("Strom-Trooper");
			auto& mComponent = Entity.AddComponent<Nexus::Component::Mesh>();
			LoadMesh("Sandbox/Assets/Meshes/StormTrooper/Stormtrooper.NxMeshAsset", mComponent);

			auto& tComponent = Entity.GetComponent<Nexus::Component::Transform>();
			tComponent.Scale = glm::vec3(0.5f, 0.5f, 0.5f);
		}
		
		{
			auto Entity = m_EditorScene->CreateEntity("House");
			auto& mComponent = Entity.AddComponent<Nexus::Component::Mesh>();
			LoadMesh("Sandbox/Assets/Meshes/House/House.NxMeshAsset", mComponent);

			auto& tComponent = Entity.GetComponent<Nexus::Component::Transform>();
			tComponent.Translation = glm::vec3(5.f, 2.5f, 0.f);
			tComponent.Scale = glm::vec3(2.5f);
		}
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
	EnvironmentBuilder::Shutdown();

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

	if (ImGui::DragFloat("Camera Speed", &speed, 1.f, 1.f, 100.f))
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

void AppLayer::SetupRenderGraph()
{
	Ref<RenderGraph> rGraph = GraphicsInterface::CreateRenderGraph();

	// Geometry pass
	{
		auto& pass = rGraph->AddRenderGraphPass("GeometryPass");

		pass.addOutput("Position")
			.set(TextureFormat::RGBA16_SFLOAT)
			.set(RenderTargetUsage::ColorAttachment);

		pass.addOutput("Normal")
			.set(TextureFormat::RGBA16_SFLOAT)
			.set(RenderTargetUsage::ColorAttachment);
		
		pass.addOutput("Albedo")
			.set(TextureFormat::SWAPCHAIN_COLOR)
			.set(RenderTargetUsage::ColorAttachment);

		pass.addOutput("Depth")
			.set(TextureFormat::SWAPCHAIN_DEPTH)
			.set(RenderTargetUsage::DepthAttachment);

		{
			pass.addGraphicsPipeline("DeferredGeometryPipeline")
				.set(Renderer::GetShaderBank()->Load("Resources/Shaders/Deferred_Geometry.glsl"))
				.set(RenderPipelineCullMode::None)
				.set(RenderPipelinePolygonMode::Fill)
				.set(RenderPipelineTopology::TriangleList)
				.set(RenderPiplineFrontFaceType::AntiClockwise)
				.set(RenderPipelineVertexInputRate::PerVertex, sizeof(Meshing::Vertex))
				.addVertexAttrib(RenderPipelineVertexAttribFormat::Vec3, 0, offsetof(Meshing::Vertex, Meshing::Vertex::position))
				.addVertexAttrib(RenderPipelineVertexAttribFormat::Vec3, 1, offsetof(Meshing::Vertex, Meshing::Vertex::normal))
				.addVertexAttrib(RenderPipelineVertexAttribFormat::Vec3, 2, offsetof(Meshing::Vertex, Meshing::Vertex::tangent))
				.addVertexAttrib(RenderPipelineVertexAttribFormat::Vec3, 3, offsetof(Meshing::Vertex, Meshing::Vertex::bitangent))
				.addVertexAttrib(RenderPipelineVertexAttribFormat::Vec3, 4, offsetof(Meshing::Vertex, Meshing::Vertex::texCoord0));
		}

	}
	
	// Lighting pass
	{
		auto& pass = rGraph->AddRenderGraphPass("LightingPass")
			.addGraphDependency("GeometryPass");

		pass.addOutput("Deferred")
			.set(TextureFormat::SWAPCHAIN_COLOR)
			.set(RenderTargetUsage::ColorAttachment);

		{
			pass.addGraphicsPipeline("DeferredLightingPipeline")
				.set(Renderer::GetShaderBank()->Load("Resources/Shaders/Deferred_Lighting.glsl"))
				.set(RenderPipelineCullMode::None)
				.set(RenderPipelinePolygonMode::Fill)
				.set(RenderPipelineTopology::TriangleList)
				.set(RenderPiplineFrontFaceType::AntiClockwise);
		}

	}

	// Fullscreen pass
	{
		auto& pass = rGraph->AddRenderGraphPass("FullScreenPass")
			.addGraphDependency("LightingPass")
			.promoteToBackBuffer();

		pass.addOutput(RENDER_TARGET_BACKBUFFER_NAME);

		{
			pass.addGraphicsPipeline("FullScreenPipeline")
				.set(Renderer::GetShaderBank()->Load("Resources/Shaders/FullScreen.glsl"))
				.set(RenderPipelineCullMode::None)
				.set(RenderPipelinePolygonMode::Fill)
				.set(RenderPipelineTopology::TriangleList)
				.set(RenderPiplineFrontFaceType::AntiClockwise);
		}

	}

	rGraph->Bake();
}