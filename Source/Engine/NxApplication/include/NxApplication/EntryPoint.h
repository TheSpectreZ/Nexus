#pragma once
#include <string>
#include <codecvt>
#include <Windows.h>

#include "Application.h"

extern Nexus::Application* CreateApplication(std::string cmdLineArg);

int APIENTRY wWinMain(HINSTANCE h1, HINSTANCE h2, LPWSTR l, int n)
{
	int wideStrLength = static_cast<int>(wcslen(l));
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, l, wideStrLength, nullptr, 0, nullptr, nullptr);

	std::string args(bufferSize, '\0');
	WideCharToMultiByte(CP_UTF8, 0, l, wideStrLength, &args[0], bufferSize, nullptr, nullptr);

	Nexus::Application* app = CreateApplication(args);

	app->Init();
	app->Run();
	app->Shut();

	delete app;

	return 0;
 }