#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

void ImGuiLayer::Attach(const Nexus::Platform::Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
   
    ImGuiIO& io = ImGui::GetIO(); 
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
    
    io.Fonts->AddFontFromFileTTF("res/fonts/Rubik-Medium.ttf", 18.f);

    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
	ImGui_ImplOpenGL3_Init("#version 130");

    Panels.Initialize();
}

void ImGuiLayer::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    Panels.Render();
  
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::Detach()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();

	ImGui::DestroyContext();
}
