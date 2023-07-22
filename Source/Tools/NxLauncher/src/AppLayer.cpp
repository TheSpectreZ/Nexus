#include "AppLayer.h"
#include "NxGraphics/TypeImpls.h"
#include "NxRenderEngine/Renderer.h"
#include "NxRenderEngine/ResourcePool.h"
#include "NxCore/Logger.h"
#include "NxCore/Input.h"
#include "NxApplication/FileDialog.h"
#include "NxApplication/Application.h"
#include "NxCore/ProjectSerializer.h"
#include "NxImGui/Context.h"
#include "imgui.h"

using namespace Nexus;

void AppLayer::OnAttach()
{
	auto& window = Application::Get()->GetWindow();
	auto rAPI = Application::Get()->GetAppSpecs().rApi;

	NxImGui::Context::Initialize(window, rAPI);
}

void AppLayer::OnUpdate(float dt)
{
	
}

void AppLayer::OnRender()
{
	NxImGui::Context::StartFrame();
	
	RenderLauncherWindow();
	
	NxImGui::Context::EndFrame();
}

void AppLayer::OnDetach()
{
	NxImGui::Context::Shutdown();
}

void AppLayer::OnWindowResize(int width, int height)
{
	Extent extent = { (uint32_t)width,(uint32_t)height };
	NxImGui::Context::OnWindowResize(extent);
}

void AppLayer::RenderLauncherWindow()
{
	ImGui::Begin("Project");

	static char buffer[50];

	ImGui::InputText("Project Name", buffer, sizeof(buffer));

	static std::string filepath;
	ImGui::LabelText("Filepath", filepath.c_str());
	ImGui::SameLine();

	static bool pathselected = false;
	if (ImGui::Button("Browse"))
	{
		std::string path = Nexus::FileDialog::SelectFolder();
		if (path.empty())
			pathselected = false;
		else
		{
			filepath = path;
			pathselected = true;
		}
	}

	ImGui::Checkbox("Enable Multisampling", &m_ProjectSpecs.renderSettings.EnableMultiSampling);

	static bool launch = false;
	ImGui::Checkbox("Launch Editor", &launch);

	if (launch)
	{
		ImGui::SameLine();
		ImGui::TextWrapped("Note that This would launch the Editor without the Debugger. Load the Project from Editor Itself after lauching it from VS to use Debugger");
	}

	if (ImGui::Button("Create"))
	{
		if (pathselected)
		{
			m_ProjectSpecs.Name = buffer;
			m_ProjectSpecs.RootPath = filepath + "\\" + buffer;

			GenerateProject(m_ProjectSpecs);

			if (launch)
			{
				LaunchEditor();
			}
			pathselected = false;
		}
	}

	if (launch && ImGui::Button("Load"))
	{
		std::string path = Nexus::FileDialog::OpenFile("Nexus Project (*.nxProject)\0*.nxProject\0");
		if (!path.empty())
		{
			Nexus::ProjectSerializer::DeSerialize(path, m_ProjectSpecs);
			LaunchEditor();
		}
	}

	ImGui::End();
}

void AppLayer::GenerateProject(const Nexus::ProjectSpecifications& specs)
{
	NEXUS_LOG("NxLauncher", "Project Specifications: %s , %s", specs.Name.c_str(), specs.RootPath.c_str());

	// Generate Premake File and batch File
	{
		std::filesystem::create_directory(specs.RootPath);

		std::string line;

		std::ifstream ini_file{ "Resources/LUA/premake5.lua" };
		std::ofstream out_file{ specs.RootPath + "/premake5.lua" };

		if (!ini_file || !out_file)
		{
			NEXUS_LOG("NxLauncher", "Cannot read File");
			return;
		}

		while (std::getline(ini_file, line))
		{
			auto startit = line.find_first_of("@");
			if (startit != std::string::npos)
			{
				auto endit = line.find_last_of("@");

				line = line.substr(0, startit) + specs.Name + line.substr(endit + 1);
			}

			out_file << line << "\n";
		}

		NEXUS_LOG("NxLauncher", "Copied Lua File");

		ini_file.close();
		out_file.close();

		if (!std::filesystem::exists(specs.RootPath + "\\GenerateProject.bat"))
		{
			std::filesystem::copy("Sandbox\\GenerateProject.bat", specs.RootPath + "\\GenerateProject.bat");
			NEXUS_LOG("NxLauncher","Copied Batch File");
		}

		std::filesystem::path dir = std::filesystem::path(specs.RootPath) / "Binaries\\premake";

		if (!std::filesystem::exists(dir))
		{
			std::filesystem::create_directories(dir);
		}

		std::filesystem::path p = specs.RootPath + "\\Binaries\\premake\\premake5.exe";

		if (!std::filesystem::exists(p))
			std::filesystem::copy_file("Sandbox\\Binaries\\premake\\premake5.exe", p);
	}

	// C# Project
	{
		auto currentPath = std::filesystem::current_path();
		std::filesystem::current_path(specs.RootPath);

		NEXUS_LOG("NxLauncher", "%s", std::filesystem::current_path().string().c_str());

		std::string p = specs.RootPath + "\\GenerateProject.bat";
		if (!std::system(p.c_str()))
		{
			std::filesystem::remove(p);
			std::filesystem::remove(specs.RootPath + "\\premake5.lua");
			std::filesystem::remove_all(specs.RootPath + "\\Binaries");
		}

		std::filesystem::current_path(specs.RootPath + "\\Scripts");
		std::ofstream batchFile("build.bat");

		std::string projectName = specs.Name + ".csproj";

		batchFile << "@echo off" << std::endl;
		batchFile << "rem Build the C# project" << std::endl;
		batchFile << " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe\" /p:Configuration=Debug " << projectName << std::endl;
		batchFile << " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe\" /p:Configuration=Release " << projectName << std::endl;
		batchFile << " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\MSBuild.exe\" /p:Configuration=Dist " << projectName << std::endl;

		batchFile.close();

		std::string f = "build.bat";
		if (!std::system(f.c_str()))
		{
			std::filesystem::remove(f);
		}

		std::filesystem::current_path(currentPath);
	}

	// Generating Files
	{
		std::filesystem::create_directory(specs.RootPath + "\\Assets");

		m_ProjectSpecs.Version = "v1.0.0";
		m_ProjectSpecs.renderSettings.EnableHDR = false;
		Nexus::ProjectSerializer::Serialize(m_ProjectSpecs);

		std::filesystem::create_directories(specs.RootPath + "\\Scripts\\Bin\\Debug");
		std::filesystem::create_directories(specs.RootPath + "\\Scripts\\Bin\\Release");
		std::filesystem::create_directories(specs.RootPath + "\\Scripts\\Bin\\Dist");
	}
}

void AppLayer::LaunchEditor()
{
	auto root = std::getenv("NEXUS_ROOT_PATH");
	std::string config;

#ifdef NEXUS_DEBUG
	config = "Debug";
#elif NEXUS_RELEASE
	config = "Release";
#elif NEXUS_DIST
	config = "Dist";
#endif // NEXUS_DEBUG

	std::string exePath = std::string(root + std::string("\\Binaries\\") + config + std::string("\\NexusEditor.exe"));

	// Command-line arguments for the executable
	std::string args = std::string(m_ProjectSpecs.RootPath + "\\" + m_ProjectSpecs.Name + ".nxProject").c_str();

	// CreateProcess variables
	STARTUPINFOA si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	std::string commandLine = exePath + " " + args.c_str();

	if (!CreateProcessA(NULL, const_cast<LPSTR>(commandLine.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		NEXUS_LOG("NxLauncher", "Failed to start Editor");
	}
	else
	{
		NEXUS_LOG("NxLauncher", "Command Line - {0}", commandLine);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}
