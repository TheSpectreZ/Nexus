#include "NxCore/Input.h"

Nexus::Module::Input* Nexus::Module::Input::s_Instance = nullptr;

void Nexus::Module::Input::Initialize(const Window& window)
{
	s_Instance = new Input;
	s_Instance->m_hwnd = window.hwnd;
}

void Nexus::Module::Input::Shutdown()
{
	s_Instance->m_hwnd = nullptr;
	delete s_Instance;
}

bool Nexus::Module::Input::IsKeyPressed(uint16_t key)
{
	return m_Keys[key];
}

bool Nexus::Module::Input::IsMouseButtonPressed(uint16_t button)
{
	return m_MouseButtons[button];
}

void Nexus::Module::Input::GetMouseCursorPosition(float* x, float* y)
{
	*x = m_CursorX;
	*y = m_CursorY;
}

void Nexus::Module::Input::SetKeyState(uint16_t key, bool state)
{
	m_Keys[key] = state;
}

void Nexus::Module::Input::SetMouseButtonState(uint16_t button, bool state)
{
	m_MouseButtons[button] = state;
}

void Nexus::Module::Input::SetCursorPositionState(float x, float y)
{
	m_CursorX = x;
	m_CursorY = y;
}
