#include "Platform/Manager.h"
#include "GLFW/glfw3.h"

#include "DebugUtils/Assert.h"
#include "DebugUtils/Logger.h"

void Nexus::Platform::Initialize()
{
	int s = glfwInit();
	NEXUS_ASSERT( (s == GLFW_FALSE)  , "GLFW Init Failed");
	NEXUS_LOG_WARN("Platform Initiazed");
}

void Nexus::Platform::Shutdown()
{
	glfwTerminate();
	NEXUS_LOG_WARN("Platform Terminated");
}

void Nexus::Platform::Manager::Create_Window(Window* window,const char* title)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window->handle = glfwCreateWindow(window->width, window->height, title, NULL, NULL);

	NEXUS_ASSERT((window->handle == nullptr), "Nexus Window Creation Failed");
	NEXUS_LOG_TRACE("Window Created: Width-{0} , Height-{1}", window->width, window->height);
}

void Nexus::Platform::Manager::Destroy_Window(Window* window)
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
