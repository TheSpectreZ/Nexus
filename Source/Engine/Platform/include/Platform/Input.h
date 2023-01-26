#pragma once
#include "Window.h"
#include "KeyCode.h"
#include <utility>

namespace Nexus
{
	namespace Platform
	{
		class NEXUS_PLATFORM_API Input
		{
			static GLFWwindow* m_window;
		public:
			static void SetContextWindow(Window* window);

			static bool IsKeyPressed(uint16_t key);
			static bool IsMouseButtonPressed(uint16_t button);

			static std::pair<float, float> GetMouseCursorPosition();
		};
	}
}