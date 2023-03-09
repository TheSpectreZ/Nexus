#pragma once
#include "Core/Application.h"

extern Nexus::Application* CreateApplication();

int EntryPoint()
{
	Nexus::Application* app = CreateApplication();

	app->Init();
	app->Run();
	app->Shut();

	delete app;

	return 0;
}

#ifndef NEXUS_DIST

int main()
{
	return EntryPoint();
}

#else

#include <Windows.h>
int APIENTRY WinMain(HINSTANCE h1, HINSTANCE h2, LPSTR l, int n)
{
	return EntryPoint();
}

#endif // NEXUS_DIST

