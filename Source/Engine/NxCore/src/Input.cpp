#include "GLFW/glfw3.h"
#include "NxCore/Input.h"

Nexus::Module::Input* Nexus::Module::Input::s_Instance = nullptr;

void Nexus::Module::Input::Initialize(const Window& window)
{
	s_Instance = new Input;
	s_Instance->m_handle = window.glfwHandle;
}

void Nexus::Module::Input::Shutdown()
{
	s_Instance->m_handle = nullptr;
	delete s_Instance;
}

bool Nexus::Module::Input::IsKeyPressed(uint16_t key, bool repeat)
{
	auto state = glfwGetKey((GLFWwindow*)m_handle, key);
	
	if (repeat)
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	
	return state == GLFW_PRESS;
}

bool Nexus::Module::Input::IsMouseButtonPressed(uint16_t button)
{
	auto state = glfwGetMouseButton((GLFWwindow*)m_handle, button);
	return state == GLFW_PRESS;
}

void Nexus::Module::Input::GetMouseCursorPosition(float* x, float* y)
{
	static double X, Y;
	glfwGetCursorPos((GLFWwindow*)m_handle, &X, &Y);
	*x = (float)X; *y = (float)Y;
}
