#include "Platform/Input.h"
#include "GLFW/glfw3.h"

GLFWwindow* Nexus::Platform::Input::m_window = nullptr;

void Nexus::Platform::Input::SetContextWindow(Window* window)
{
	m_window = window->handle;
}

bool Nexus::Platform::Input::IsKeyPressed(uint16_t key)
{
	auto state = glfwGetKey(m_window, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Nexus::Platform::Input::IsMouseButtonPressed(uint16_t button)
{
	return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}
