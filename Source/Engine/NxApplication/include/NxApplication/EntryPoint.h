#pragma once
#include <string>
#include <codecvt>
#include <Windows.h>

#include "NxCore/Base.h"
#include "Application.h"

extern Nexus::Application* CreateApplication(const std::unordered_map<std::string, std::string>& ccMap);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	auto args = Nexus::BuildString(lpCmdLine);
	auto CCmap = Nexus::BuildCCMap(args);

	auto app = CreateApplication(CCmap);

	app->Init(CCmap);
	app->Run();
	app->Shut();

	delete app;

	return 0;
 }