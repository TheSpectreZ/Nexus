#include "EditorLayer.h"
#include "NxGameEngine/Engine.h"

#include "imgui.h"

void Nexus::EditorLayer::Create()
{
	auto gd = this->getGraphicsDevice();

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

	mUserInterface = gd->CreateUserInterface();
	mSceneDrawer = std::make_shared<SceneDrawer>(gd);
}

void Nexus::EditorLayer::Update(float dt)
{
}

void Nexus::EditorLayer::Render()
{
	auto gd = this->getGraphicsDevice();
	
	{
		gd->BeginFrame();

		mSceneDrawer->Draw();
		RenderImGui();

		auto presentable = mUserInterface->getPresentable();
		gd->EndFrame(presentable.first, presentable.second);
	}
}

void Nexus::EditorLayer::Destroy()
{
	mSceneDrawer.reset();
	mUserInterface.reset();

	ImGui::DestroyContext();
}

void Nexus::EditorLayer::OnWindowResize(int width, int height)
{
	mSceneDrawer->OnWindowResize(width, height);
	mUserInterface->OnWindowResize(width, height);
}

void Nexus::EditorLayer::OnKeyPress(Key::Code key, InputAction action, InputModifier mods)
{
}

void Nexus::EditorLayer::OnMousePress(Mouse::Code mouse, InputAction action, InputModifier mods)
{
}

void Nexus::EditorLayer::RenderImGui()
{
	mUserInterface->Begin();
	{
		ImGui::DockSpaceOverViewport();
		ImGui::ShowDemoWindow();
	}
	mUserInterface->End();
}
