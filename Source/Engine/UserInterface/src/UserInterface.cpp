#include "UserInterface/UserInterface.h"
#include "Utility/Logger.h"
#include "imgui.h"
#include "UserInterface/Manager.h"

void Nexus::UserInterface::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	NEXUS_LOG_WARN("User Interface Initialized");

	Manager* m = Manager::Get();
	m = new Manager();
}

void Nexus::UserInterface::Shutdown()
{
	delete Manager::Get();

	ImGui::DestroyContext();
	NEXUS_LOG_WARN("User Interface Terminated");
}
