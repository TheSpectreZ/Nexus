#ifndef NEXUS_DIST

#define ENTRY_POINT int main()

#else

#include <Windows.h>
int WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int n)

#endif // NEXUS_DIST

#include "Launcher.h"

ENTRY_POINT
{
	Launcher launcher;

	launcher.Initialize();
	launcher.Run();
	launcher.Shutdown();

}