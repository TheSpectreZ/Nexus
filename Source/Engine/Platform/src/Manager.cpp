#include "Platform/Manager.h"
#include "GLFW/glfw3.h"

#include "Utility/Assert.h"
#include "Utility/Logger.h"

float Nexus::Platform::Manager::deltaTime = 0.f;

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

void Nexus::Platform::Manager::Create_Window(Window& window)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window.handle = glfwCreateWindow(window.width, window.height, window.title, NULL, NULL);

	NEXUS_ASSERT((window.handle == nullptr), "Nexus Window Creation Failed");
	NEXUS_LOG_TRACE("Window Created: Width-{0} , Height-{1}", window.width, window.height);
}

void Nexus::Platform::Manager::Destroy_Window(const Window& window)
{
	glfwDestroyWindow(window.handle);
}

void Nexus::Platform::Manager::Close(const Window& window)
{
	glfwSetWindowShouldClose(window.handle, GLFW_TRUE);
}

bool Nexus::Platform::Manager::IsOpen(const Window& window)
{
	return !glfwWindowShouldClose(window.handle);
}

void Nexus::Platform::Manager::PollEvents()
{
	static float ct, lt;

	ct = (float)glfwGetTime();
	deltaTime = ct - lt;
	lt = ct;

	glfwPollEvents();
}
