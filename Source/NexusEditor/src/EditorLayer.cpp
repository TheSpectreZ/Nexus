#include "EditorLayer.h"

void EditorLayer::OnAttach()
{
	NEXUS_LOG_DEBUG("Editor Layer Attached");

	// Default Sandbox Project
	{
		Nexus::ProjectSerializer::DeSerialize("D:\\EngineDev\\SandboxProject\\SandboxProject.nxProject", m_ProjectSpecs);
	}

	CreateRenderpassAndFramebuffers();

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

	Nexus::Ref<Nexus::Shader> simpleShader = Nexus::ShaderLib::Get("shaders/pbr.shader");
	
	// Pipeline
	{
		Nexus::PipelineCreateInfo Info{};
		Info.shader = simpleShader;
		Info.subpass = 0;
		Info.renderpass = m_GraphicsPass;
		Info.multisampled = m_ProjectSpecs.renderSettings.EnableMultiSampling;

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

	// Camera
	{
		using namespace Nexus;

		m_cameraController.AttachCamera(&m_camera);
		m_cameraController.SetSpeed(5.f);
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
		
		m_EditorScene = Nexus::Scene::Create();
		m_CurrentScene = m_EditorScene;

		Nexus::Entity e2 = m_EditorScene->CreateEntity("Cube");
		//e2.AddComponent<Nexus::Component::Mesh>();
		e2.AddComponent<Nexus::Component::BoxCollider>();
		e2.AddComponent<Nexus::Component::RigidBody>();
		
		m_SceneData = Nexus::SceneBuildData::Build(m_EditorScene, simpleShader);
		m_SceneRenderer.SetContext(m_EditorScene, m_SceneData);
		
		m_PhysicsWorld = Nexus::PhysicsWorld::Create();
	}
	
	// Editor
	{
		Nexus::EditorContext::Initialize(m_ImGuiPass);

		m_ImGuiEditorViewport.Initialize();
		m_ImGuiEditorViewport.SetContext(m_GraphicsFramebuffer, 2);
		
		m_SceneHeirarchy.SetContext(m_SceneData, m_EditorScene);
		
		m_ContentBrowser.Initialize();
		m_ContentBrowser.SetContext(m_ProjectSpecs.RootPath);
	}
}

void EditorLayer::OnUpdate(Nexus::Timestep ts)
{
	glm::vec2 size = m_ImGuiEditorViewport.GetViewportSize();
	if (size != m_ImGuiEditorViewportSize)
	{
		m_ImGuiEditorViewportSize = size;
		m_cameraController.SetPerspectiveProjection(45.f, size.x, size.y, 0.1f, 1000.f);
	}

	m_cameraController.Move();

	m_SceneData->Update(m_CurrentScene, m_camera);

	if (m_IsScenePlaying && !m_PauseScene)
	{
		Nexus::ScriptEngine::OnSceneUpdate(ts.GetSeconds());
		m_PhysicsWorld->OnSceneUpdate(ts.GetSeconds());
	}
}

void EditorLayer::OnRender()
{
	// Graphics
	{
		Nexus::Renderer::BeginRenderPass(m_GraphicsPass, m_GraphicsFramebuffer);

		Nexus::Renderer::BindPipeline(m_Pipeline);

		Nexus::Renderer::SetViewport(m_viewport);
		Nexus::Renderer::SetScissor(m_scissor);

		m_SceneRenderer.Render();

		Nexus::Renderer::EndRenderPass();
	}

	// ImGui
	{
		Nexus::Renderer::BeginRenderPass(m_ImGuiPass, m_ImGuiFramebuffer);
		Nexus::EditorContext::StartFrame();
		
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		m_SceneHeirarchy.Render();
		m_ContentBrowser.Render();
		m_ImGuiEditorViewport.Render();

		RenderEditorMainMenu();
		RenderEditorWorldControls();

		Nexus::EditorContext::Render();
		Nexus::Renderer::EndRenderPass();
	}
}

void EditorLayer::OnDetach()
{
	if (m_IsScenePlaying)
	{
		m_SceneRenderer.SetContext(m_EditorScene, m_SceneData);
		m_CurrentScene = m_EditorScene;

		Nexus::ScriptEngine::OnSceneStop();
		m_PhysicsWorld->OnSceneStop();
		m_IsScenePlaying = false;

		m_RuntimeScene->Clear();
		
		NEXUS_LOG_WARN("Stopped Scene Runtime");
	}

	m_SceneData->Destroy();
	m_EditorScene->Clear();

	if (m_RuntimeScene)
		m_RuntimeScene->Clear();

	Nexus::EditorContext::Shutdown();
	NEXUS_LOG_DEBUG("Editor Layer Detached");
}

void EditorLayer::OnWindowResize(int width, int height)
{
	Nexus::Extent Extent = Nexus::Renderer::GetSwapchain()->GetExtent();
	
	// Screen
	{
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

	m_GraphicsFramebuffer.reset();
	m_ImGuiFramebuffer.reset();

	m_GraphicsFBspecs.extent = Extent;
	m_GraphicsFramebuffer = Nexus::Framebuffer::Create(m_GraphicsFBspecs);

	m_ImGuiFBspecs.extent = Extent;
	m_ImGuiFramebuffer = Nexus::Framebuffer::Create(m_ImGuiFBspecs);

	m_ImGuiEditorViewport.SetContext(m_GraphicsFramebuffer, 2);
}

void EditorLayer::CreateRenderpassAndFramebuffers()
{
	// Graphics Renderpass
	{
		std::vector<Nexus::RenderpassAttachmentDescription> attachments;
		{
			auto& color = attachments.emplace_back();
			color.type = Nexus::ImageType::Color;
			color.multiSampled = m_ProjectSpecs.renderSettings.EnableMultiSampling;
			color.hdr = m_ProjectSpecs.renderSettings.EnableHDR;
			color.load = Nexus::ImageOperation::Clear;
			color.store = Nexus::ImageOperation::Store;
			color.initialLayout = Nexus::ImageLayout::Undefined;
			color.finalLayout = Nexus::ImageLayout::ColorAttachment;

			auto& depth = attachments.emplace_back();
			depth.type = Nexus::ImageType::Depth;
			depth.multiSampled = m_ProjectSpecs.renderSettings.EnableMultiSampling;
			depth.hdr = false;
			depth.load = Nexus::ImageOperation::Clear;
			depth.store = Nexus::ImageOperation::DontCare;
			depth.initialLayout = Nexus::ImageLayout::Undefined;
			depth.finalLayout = Nexus::ImageLayout::DepthStencilAttachment;

			auto& resolve = attachments.emplace_back();
			resolve.type = Nexus::ImageType::Resolve;
			resolve.multiSampled = false;
			resolve.hdr = m_ProjectSpecs.renderSettings.EnableHDR;
			resolve.load = Nexus::ImageOperation::DontCare;
			resolve.store = Nexus::ImageOperation::Store;
			resolve.initialLayout = Nexus::ImageLayout::Undefined;
			resolve.finalLayout = Nexus::ImageLayout::ShaderReadOnly;
		}

		std::vector<Nexus::SubpassDescription> subpasses;
		{
			auto& subpass0 = subpasses.emplace_back();
			subpass0.ColorAttachments = { 0 };
			subpass0.DepthAttachment = 1;
			subpass0.ResolveAttachment = 2;
		}

		std::vector<Nexus::SubpassDependency> subpassDependencies;
		{
			auto& dep = subpassDependencies.emplace_back();
			dep.srcSubpass = Nexus::SubpassDependency::ExternalSubpass;
			dep.dstSubpass = 0;
			dep.srcStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.dstStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.srcAccessFlags = Nexus::AccessFlag::None;
			dep.dstAccessFlags = Nexus::AccessFlag::ColorAttachmentWrite;
		}

		Nexus::RenderpassSpecification specs{};
		specs.attachments = &attachments;
		specs.subpasses = &subpasses;
		specs.dependencies = &subpassDependencies;

		m_GraphicsPass = Nexus::Renderpass::Create(specs);
	}

	// Imgui Renderpass
	{
		std::vector<Nexus::RenderpassAttachmentDescription> attachments;
		{
			auto& color = attachments.emplace_back();
			color.type = Nexus::ImageType::Color;
			color.multiSampled = false;
			color.hdr = false;
			color.load = Nexus::ImageOperation::Clear;
			color.store = Nexus::ImageOperation::Store;
			color.initialLayout = Nexus::ImageLayout::Undefined;
			color.finalLayout = Nexus::ImageLayout::PresentSrc;
		}

		std::vector<Nexus::SubpassDescription> subpasses;
		{
			auto& subpass0 = subpasses.emplace_back();
			subpass0.ColorAttachments = { 0 };
		}

		std::vector<Nexus::SubpassDependency> subpassDependencies;
		{
			auto& dep = subpassDependencies.emplace_back();
			dep.srcSubpass = Nexus::SubpassDependency::ExternalSubpass;
			dep.dstSubpass = 0;
			dep.srcStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.dstStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.srcAccessFlags = Nexus::AccessFlag::None;
			dep.dstAccessFlags = Nexus::AccessFlag::ColorAttachmentWrite;
		}

		Nexus::RenderpassSpecification specs{};
		specs.attachments = &attachments;
		specs.subpasses = &subpasses;
		specs.dependencies = &subpassDependencies;

		m_ImGuiPass = Nexus::Renderpass::Create(specs);
	}

	// Graphics Framebuffer
	{
		auto extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		auto& a1 = m_GraphicsFBspecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::Color;
		a1.multisampled = m_ProjectSpecs.renderSettings.EnableMultiSampling;
		a1.hdr = m_ProjectSpecs.renderSettings.EnableHDR;

		auto& a2 = m_GraphicsFBspecs.attachments.emplace_back();
		a2.Type = Nexus::FramebufferAttachmentType::DepthStencil;
		a2.multisampled = m_ProjectSpecs.renderSettings.EnableMultiSampling;
		a2.hdr = false;

		auto& a3 = m_GraphicsFBspecs.attachments.emplace_back();
		a3.Type = Nexus::FramebufferAttachmentType::ShaderReadOnly_Color;
		a3.multisampled = false;
		a3.hdr = m_ProjectSpecs.renderSettings.EnableHDR;

		m_GraphicsFBspecs.extent = extent;
		m_GraphicsFBspecs.renderpass = m_GraphicsPass;

		m_GraphicsFramebuffer = Nexus::Framebuffer::Create(m_GraphicsFBspecs);
	}

	// ImGui Framebuffer
	{
		auto extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		auto& a1 = m_ImGuiFBspecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::PresentSrc;
		a1.multisampled = false;
		a1.hdr = false;

		m_ImGuiFBspecs.extent = extent;
		m_ImGuiFBspecs.renderpass = m_ImGuiPass;

		m_ImGuiFramebuffer = Nexus::Framebuffer::Create(m_ImGuiFBspecs);
	}
}

void EditorLayer::RenderEditorMainMenu()
{
	ImGui::BeginMainMenuBar();
	
	if(ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("New Scene");
		ImGui::MenuItem("Open Scene");
		ImGui::MenuItem("Save Scene");
		
		ImGui::EndMenu();
	}
	
	if(ImGui::BeginMenu("Scripting"))
	{
		if (ImGui::MenuItem("Play Scene"))
		{
			if (!m_IsScenePlaying)
			{
				m_RuntimeScene = m_EditorScene->Duplicate();
				m_CurrentScene = m_RuntimeScene;

				m_SceneRenderer.SetContext(m_RuntimeScene, m_SceneData);

				Nexus::ScriptEngine::OnSceneStart(m_RuntimeScene);
				m_PhysicsWorld->OnSceneStart(m_RuntimeScene);
				m_IsScenePlaying = true;
				m_PauseScene = false;

				NEXUS_LOG_WARN("Started Scene Runtime");
			}
		}

		if (ImGui::MenuItem("Stop Scene"))
		{
			if (m_IsScenePlaying)
			{
				m_SceneRenderer.SetContext(m_EditorScene, m_SceneData);
				m_CurrentScene = m_EditorScene;

				Nexus::ScriptEngine::OnSceneStop();
				m_PhysicsWorld->OnSceneStop();
				m_IsScenePlaying = false;

				m_RuntimeScene->Clear();
				m_RuntimeScene.reset();

				NEXUS_LOG_WARN("Stopped Scene Runtime");
			}
		}

		if (ImGui::MenuItem("Pause Scene"))
		{
			m_PauseScene = !m_PauseScene;
		}
		
		if (ImGui::MenuItem("Reload Assembly"))
		{
			Nexus::ScriptEngine::ReloadAssembly();
		}
		
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void EditorLayer::RenderEditorWorldControls()
{
	ImGui::Begin("World");
	
	if (ImGui::BeginTabBar("World"))
	{
		if (ImGui::BeginTabItem("Scene"))
		{
			static float camSpeed = 5.f;
			if (ImGui::DragFloat("Camera Speed", &camSpeed, 1.f, 1.f, 50.f))
				m_cameraController.SetSpeed(camSpeed);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Physics"))
		{
			static glm::vec3 gravity;
			if (Nexus::ImGuiUtils::DrawVec3Control("Gravity", gravity, 0.f))
			{
				m_PhysicsWorld->SetGravity(gravity);
			}
			
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}
