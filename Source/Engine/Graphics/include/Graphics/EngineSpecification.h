#pragma once
#include "Build.h"
#include "Platform/Window.h"

namespace Nexus
{
	namespace Graphics
	{
		struct NEXUS_GRAPHICS_API EngineSpecification
		{
			Platform::Window* targetWindow;
		};
	}
}