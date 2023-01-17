#pragma once
#include "Build.h"

typedef struct GLFWwindow GLFWwindow;

namespace Nexus
{
	namespace Platform
	{
		struct NEXUS_PLATFORM_API Window
		{
			int width, height;
			GLFWwindow* handle;
		};
	}
}