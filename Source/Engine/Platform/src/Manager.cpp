#include "Platform/Manager.h"
#include "GLFW/glfw3.h"

void Nexus::Platform::Initialize()
{
	glfwInit();
}

void Nexus::Platform::Shutdown()
{
	glfwTerminate();
}

void Nexus::Platform::Manager::CreateWindow(Window* window,const char* title)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window->handle = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
}

void Nexus::Platform::Manager::DestroyWindow(Window* window)
{
	glfwDestroyWindow(window->handle);
}

void Nexus::Platform::Manager::Close(Window* window)
{
	glfwSetWindowShouldClose(window->handle, GLFW_TRUE);
}

bool Nexus::Platform::Manager::IsOpen(Window* window)
{
	return !glfwWindowShouldClose(window->handle);
}

void Nexus::Platform::Manager::PollEvents()
{
	glfwPollEvents();
}
