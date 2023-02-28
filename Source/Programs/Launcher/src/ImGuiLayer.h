#pragma once
#include "Platform/Window.h"

#include "Panels.h"

class ImGuiLayer
{
public:
	void Attach(const Nexus::Platform::Window& window);
	void Update();
	void Detach();
private:
	PanelRenderer Panels;
};

