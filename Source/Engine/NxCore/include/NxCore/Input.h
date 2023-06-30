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

		bool IsKeyPressed(uint16_t key);
		bool IsMouseButtonPressed(uint16_t button);
		void GetMouseCursorPosition(float* x, float* y);

		void SetKeyState(uint16_t key, bool state);
		void SetMouseButtonState(uint16_t button, bool state);
		void SetCursorPositionState(float x, float y);
	private:
		float m_CursorX, m_CursorY;
		void* m_hwnd;

		std::bitset<256> m_Keys;
		std::bitset<8> m_MouseButtons;
	};
}
