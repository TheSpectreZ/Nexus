#pragma once
#include <Windows.h>

namespace Nexus
{
	struct Window
	{
		int width, height;
		const char* title;
		HWND handle;
	};
}