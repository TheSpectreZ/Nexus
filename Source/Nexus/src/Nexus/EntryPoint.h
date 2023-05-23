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
	return EntryPoint(0,nullptr);
}

#endif // NEXUS_DIST

