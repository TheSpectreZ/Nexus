#pragma once
#include "Core/Application.h"

extern Nexus::Application* CreateApplication(int argc,char** argv);

int EntryPoint(int argc,char** argv)
{
	Nexus::Application* app = CreateApplication(argc, argv);

	app->Init();
	app->Run();
	app->Shut();

	delete app;

	return 0;
}

#ifndef NEXUS_DIST

int main(int argc, char** argv)
{
	return EntryPoint(argc, argv);
}

#else

#include <Windows.h>
#include <string>
#include <vector>

int APIENTRY WinMain(HINSTANCE h1, HINSTANCE h2, LPSTR l, int n)
{
    int argc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    char** argv = new char* [argc];

    // Convert wide strings to narrow strings and store them in argv
    for (int i = 0; i < argc; ++i)
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        argv[i] = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], len, nullptr, nullptr);
    }

    // Clean up the allocated memory
    LocalFree(wargv);

    int r = EntryPoint(argc, argv);

    for (int i = 0; i < argc; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;

    return r;
}

#endif // NEXUS_DIST

