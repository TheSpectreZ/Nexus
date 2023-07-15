#pragma once
#include <cstdint>
#include <bitset>

#include "Window.h"
#include "KeyCode.h"

namespace Nexus::Module
{
	class NEXUS_CORE_API Input
	{
		static Input* s_Instance;
	public:
		static Input* Get() { return s_Instance; }
		static void Initialize(const Window& window);
		static void Shutdown();

		bool IsKeyPressed(uint16_t key, bool repeat = true);
		bool IsMouseButtonPressed(uint16_t button);
		void GetMouseCursorPosition(float* x, float* y);
	private:
		void* m_handle;
	};
}
