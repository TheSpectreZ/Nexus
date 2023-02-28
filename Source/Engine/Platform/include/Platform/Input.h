#pragma once
#include "Window.h"
#include "KeyCode.h"
#include <utility>

namespace Nexus
{
	namespace Platform
	{
		namespace Input
		{
			void NEXUS_PLATFORM_API SetContextWindow(const Window& window);

			bool NEXUS_PLATFORM_API IsKeyPressed(uint16_t key);
			bool NEXUS_PLATFORM_API IsMouseButtonPressed(uint16_t button);

			std::pair<float, float> NEXUS_PLATFORM_API GetMouseCursorPosition();
		}
	}
}