#include "NxCore/Window.h"
#include "NxCore/KeyCode.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

static Nexus::InputAction GetInputAction(int action)
{
	switch (action)
	{
	case GLFW_PRESS:
		return Nexus::InputAction::Press;
	case GLFW_REPEAT:
		return Nexus::InputAction::Repeat;
	default:
		return Nexus::InputAction::Release;
	}
}

static Nexus::InputModifier GetInputMods(int mods)
{
	switch (mods)
	{
	case GLFW_MOD_SHIFT:
		return Nexus::InputModifier::Shift;
	case GLFW_MOD_CONTROL:
		return Nexus::InputModifier::Control;
	case GLFW_MOD_ALT:
		return Nexus::InputModifier::Alt;
	case GLFW_MOD_SUPER:
		return Nexus::InputModifier::Super;
	case GLFW_MOD_CAPS_LOCK:
		return Nexus::InputModifier::CapsLock;
	case GLFW_MOD_NUM_LOCK:
		return Nexus::InputModifier::NumLock;
	default:
		return Nexus::InputModifier::Invalid;
	}
}

void Nexus::Windowing::Init()
{
	glfwInit();
}

void Nexus::Windowing::Shut()
{
	glfwTerminate();
}

bool Nexus::Windowing::Create(Window* pInWindow)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	auto window = glfwCreateWindow(pInWindow->width, pInWindow->height, pInWindow->title, nullptr, nullptr);
	if (!window) 
		return false;

	pInWindow->nativeHandle = glfwGetWin32Window(window);
	pInWindow->glfwHandle = window;

	glfwSetWindowUserPointer(window, pInWindow);

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			auto data = (Window*)glfwGetWindowUserPointer(window);
			data->width = width;
			data->height = height;

			data->eventHandler.OnResize(width, height);
		});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto data = (Window*)glfwGetWindowUserPointer(window);
			data->eventHandler.OnKeyPress(Key::Code(key), GetInputAction(action), GetInputMods(mods));
		});
	
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int key, int action, int mods)
		{
			auto data = (Window*)glfwGetWindowUserPointer(window);
			data->eventHandler.OnMousePress(Mouse::Code(key), GetInputAction(action), GetInputMods(mods));
		});

	return true;
}

void Nexus::Windowing::Destroy(const Window* pWindow)
{
	glfwDestroyWindow((GLFWwindow*)pWindow->glfwHandle);
}

bool Nexus::Windowing::IsOpen(const Window* pWindow)
{
	return !glfwWindowShouldClose((GLFWwindow*)pWindow->glfwHandle);
}

void NEXUS_CORE_API Nexus::Windowing::Show(Window* pWindow)
{
	glfwShowWindow((GLFWwindow*)pWindow->glfwHandle);
}

void Nexus::Windowing::PollEvents()
{
	glfwPollEvents();
}

double Nexus::Windowing::GetDeltaTime()
{
	static double ct = 0.0, lt = 0.0, dt = 0.0;
	
	ct = glfwGetTime();
	dt = ct - lt;
	lt = ct;

	return dt;
}
