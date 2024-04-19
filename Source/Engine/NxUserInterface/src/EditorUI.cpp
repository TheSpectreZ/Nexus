#include "NxUserInterface/EditorUI.h"
#include "imgui.h"

Nexus::EditorUIDrawer::EditorUIDrawer(GraphicsDevice* pDevice)
	:Drawer(pDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.f;
	}

	mUserInterface = pDevice->CreateUserInterface();
}

Nexus::EditorUIDrawer::~EditorUIDrawer()
{
	mUserInterface.reset();
	ImGui::DestroyContext();
}

void Nexus::EditorUIDrawer::Draw()
{
}

Nexus::Ref<Nexus::Framebuffer> Nexus::EditorUIDrawer::getPresentableFramebuffer()
{
	return Ref<Framebuffer>();
}
