#pragma once
#include "Platform/Window.h"
#include "ImGuiLayer.h"

class Launcher
{
public:
	void Initialize();
	void Run();
	void Shutdown();
private:
	Nexus::Platform::Window m_Window;
	ImGuiLayer m_imguiLayer;
};

