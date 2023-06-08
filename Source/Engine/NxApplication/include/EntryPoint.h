#pragma once
#include <string>
#include <codecvt>
#include <Windows.h>

#include "Application.h"

extern Nexus::Application* CreateApplication(std::string cmdLineArg);

int APIENTRY wWinMain(HINSTANCE h1, HINSTANCE h2, LPWSTR l, int n)
{
    std::string args = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(l);
	
	Nexus::Application* app = CreateApplication(args);

	app->Init();
	app->Run();
	app->Shut();

	delete app;

	return 0;
 }