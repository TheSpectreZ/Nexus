#include "Launcher.h"

#include "Utility/Logger.h"
#include "Utility/Assert.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Platform/Manager.h"
#include "Platform/Input.h"
#include "Platform/FileDialog.h"

using namespace Nexus;

void Launcher::Initialize()
{
	NEXUS_LOG_INIT

	Platform::Initialize();

	m_Window.width = 1280;
	m_Window.height = 720;
	m_Window.title = "Nexus Launcher";
	m_Window.type = Platform::Window::Type::OPENGL;

	Platform::Manager::Create_Window(m_Window);

	glfwMakeContextCurrent(m_Window.handle);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	
	Platform::Input::SetContextWindow(m_Window);
	Platform::FileDialog::SetContextWindow(m_Window);

	m_imguiLayer.Attach(m_Window);
}

void Launcher::Run()
{
	while (Platform::Manager::IsOpen(m_Window))
	{
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		Platform::Manager::PollEvents();

		m_imguiLayer.Update();

		glfwSwapBuffers(m_Window.handle);
	}
}

void Launcher::Shutdown()
{
	m_imguiLayer.Detach();

	Platform::Manager::Destroy_Window(m_Window);
	
	Platform::Shutdown();

	NEXUS_LOG_SHUT
}
