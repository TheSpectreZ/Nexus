#pragma once
#include "Application.h"

extern Nexus::Application* CreateApplication();

int main()
{
	Nexus::Application* app = CreateApplication();

	app->Run();

	delete app;
}