#pragma once
#include "Core/Application.h"

extern Nexus::Application* CreateApplication(std::string cmdLineArg);

int EntryPoint(std::string cmdLineArg)
{
	Nexus::Application* app = CreateApplication(cmdLineArg);

	app->Init();
	app->Run();
	app->Shut();

	delete app;

	return 0;
}

#ifndef NEXUS_DIST

int main(int argc, char** argv)
{
    std::string args;
    if (argc > 1)
        args = argv[1];

	return EntryPoint(args);
}

#else

#include <Windows.h>
#include <codecvt>

int APIENTRY wWinMain(HINSTANCE h1, HINSTANCE h2, LPWSTR l, int n)
{
    std::string args = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(l);
	return EntryPoint(args);
 }

#endif // NEXUS_DIST

