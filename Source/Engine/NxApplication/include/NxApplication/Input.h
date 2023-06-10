#pragma once
#include "NxCore/Window.h"
#include "KeyCode.h"
#include <utility>

namespace Nexus
{
	namespace Input
	{
		void SetContextWindow(const Window& window);

		bool IsKeyPressed(uint16_t key);
		bool IsMouseButtonPressed(uint16_t button);

		std::pair<float, float> GetMouseCursorPosition();
	}
}