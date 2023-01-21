#pragma once
#include "Build.h"

typedef struct GLFWwindow GLFWwindow;

namespace Nexus
{
	namespace Graphics
	{
		struct NEXUS_GRAPHICS_API EngineSpecification
		{
			GLFWwindow* windowHandle;
		};
	}
}