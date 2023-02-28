#ifndef NEXUS_DIST

#define ENTRY_POINT int main()

#else

#include <Windows.h>
int WinMain(HINSTANCE h, HINSTANCE p, LPSTR c, int n)

#endif // NEXUS_DIST

#include "Editor.h"

ENTRY_POINT
{
	Editor editor;

	editor.Initialize();
	editor.Run();
	editor.Shutdown();
}