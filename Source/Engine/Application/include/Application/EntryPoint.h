#pragma once
#include "Application.h"

extern Nexus::Application* CreateApplication();

#ifdef WINDOWED

#include <Windows.h>
int WinMain(HINSTANCE h,HINSTANCE p,LPSTR c,int n)

#else

int main()

#endif // WINDOWED

{
	Nexus::Application* app = CreateApplication();

	app->Run();

	delete app;
}