#include "nxpch.h"
#include "Input.h"
#include "GLFW/glfw3.h"

namespace Nexus::Input
{
	GLFWwindow* m_window = nullptr;
}

void Nexus::Input::SetContextWindow(const Window& window)
{
	m_window = window.handle;
}

bool Nexus::Input::IsKeyPressed(uint16_t key)
{
	auto state = glfwGetKey(m_window, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Nexus::Input::IsMouseButtonPressed(uint16_t button)
{
	return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

std::pair<float, float> Nexus::Input::GetMouseCursorPosition()
{
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return { (float)x,(float)y };
}
