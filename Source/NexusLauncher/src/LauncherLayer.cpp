#include "LauncherLayer.h"

#include <fstream>

void LauncherLayer::OnAttach()
{
	// Renderpass
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

		m_Renderpass = Nexus::Renderpass::Create(specs);
	}

	// Framebuffer
	{
		auto extent = Nexus::Renderer::GetSwapchain()->GetExtent();

		auto& a1 = m_FramebufferSpecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::PresentSrc;
		a1.multisampled = false;
		a1.hdr = false;

		m_FramebufferSpecs.extent = extent;
		m_FramebufferSpecs.renderpass = m_Renderpass;

		m_Framebuffer = Nexus::Framebuffer::Create(m_FramebufferSpecs);
	}

	Nexus::EditorContext::Initialize(m_Renderpass);
}

void LauncherLayer::OnUpdate(Nexus::Timestep step)
{
}

void LauncherLayer::OnRender()
{
	Nexus::Renderer::BeginRenderPass(m_Renderpass, m_Framebuffer);
	Nexus::EditorContext::StartFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	RenderLauncherWindow();

	Nexus::EditorContext::Render();
	Nexus::Renderer::EndRenderPass();
}

void LauncherLayer::OnDetach()
{
	Nexus::EditorContext::Shutdown();
}

void LauncherLayer::OnWindowResize(int width, int height)
{
	Nexus::Extent Extent = Nexus::Renderer::GetSwapchain()->GetExtent();
	
	m_FramebufferSpecs.extent = Extent;
	m_Framebuffer.reset();
	m_Framebuffer = Nexus::Framebuffer::Create(m_FramebufferSpecs);
}

void LauncherLayer::RenderLauncherWindow()
{
	ImGui::Begin("Project");
	
	static char buffer[50];

	ImGui::InputText("Project Name", buffer, sizeof(buffer));

	static std::string filepath;
	ImGui::LabelText("Filepath", filepath.c_str());
	ImGui::SameLine();
	
	if (ImGui::Button("Browse"))
	{
		std::string path = Nexus::FileDialog::SelectFolder();
		if (!path.empty())
		{
			filepath = path;
		}
	}
	
	if (ImGui::Button("Create"))
	{
		m_ProjectSpecs.name = buffer;
		m_ProjectSpecs.filepath = filepath + "\\" + buffer;
	
		// Setup python script to copy premake to Resources
		// Create .lua and .bat file ( Premake Exists in Bin Folder for Resources )
		// Call .bat file to create project

		GenerateProject(m_ProjectSpecs);
	}
	
	ImGui::End();
}

void LauncherLayer::GenerateProject(const ProjectSpecifications& specs)
{
	NEXUS_LOG_DEBUG("Project Specifications: {0} , {1}", specs.name, specs.filepath);

	// Generate Premake File and batch File
	{
		_mkdir(specs.filepath.c_str());

		std::string line;

		std::ifstream ini_file{ "Resources/Scripts/premake5.lua" };
		std::ofstream out_file{ specs.filepath + "/premake5.lua" };

		if (!ini_file || !out_file)
		{
			NEXUS_LOG_ERROR("Cannot read File");
		}

		while (std::getline(ini_file, line))
		{
			auto startit = line.find_first_of("@");
			if (startit != std::string::npos)
			{
				auto endit = line.find_last_of("@");

				line = line.substr(0, startit) + specs.name + line.substr(endit + 1);
			}

			out_file << line << "\n";
		}

		NEXUS_LOG_TRACE("Copied Lua File");

		ini_file.close();
		out_file.close();

		ini_file = std::ifstream("Resources/Scripts/GenerateProjects.bat");
		out_file = std::ofstream(specs.filepath + "/GenerateProject.bat");

		if (!ini_file || !out_file)
		{
			NEXUS_LOG_ERROR("Cannot read File");
		}

		while (std::getline(ini_file, line))
		{
			out_file << line << "\n";
		}

		NEXUS_LOG_TRACE("Copied Batch File");
	}

	// Generating Project
	{

	}
}
