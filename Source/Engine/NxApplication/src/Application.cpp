#include "GLFW/glfw3.h"

// STL
#include <vector>

// Application
#include "Application.h"
#include "Input.h"
#include "FileDialog.h"

// Core
#include "Logger.h"
#include "Assert.h"

// Modules
#include "RenderEngine.h"

namespace Nexus
{
	Application* Application::s_Instance = nullptr;	

	static std::vector<Nexus::Layer*> m_layerStack;
	static float s_DeltaTime = 0.f;
}

Nexus::Application::Application()
{
	s_Instance = this;
	NEXUS_LOG_INIT

	if (!glfwInit())
	{
		NEXUS_ASSERT(1, "glfw Initialization Failed");
	}

	NEXUS_LOG_TRACE("Nexus::Core Initialized");
}

Nexus::Application::~Application()
{
	glfwTerminate();
	NEXUS_LOG_TRACE("Nexus::Core Terminated");
	NEXUS_LOG_SHUT
}

void Nexus::Application::Init()
{
	// Window Creation
	{
		m_Window.width = m_AppSpecs.Window_Width;
		m_Window.height = m_AppSpecs.Window_height;
		m_Window.title = m_AppSpecs.Window_Title;
		m_Window.handle = nullptr;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		m_Window.handle = glfwCreateWindow(m_Window.width, m_Window.height, m_Window.title, nullptr, nullptr);
		NEXUS_LOG_TRACE("{2} Window Created: {0}x{1}", m_Window.width, m_Window.height, m_Window.title);

		// Callbacks
		{
			glfwSetWindowUserPointer(m_Window.handle, &m_Window);
			glfwSetWindowSizeCallback(m_Window.handle, [](GLFWwindow* window, int width, int height)
				{
					Window& data = *(Window*)glfwGetWindowUserPointer(window);
					data.width = width;
					data.height = height;
				});
		}

		Input::SetContextWindow(m_Window);
		FileDialog::SetContextWindow(m_Window);
	}

	// Modules
	{
		RenderEngine::Initialize(m_AppSpecs.rApi);
	}
}

void Nexus::Application::Run()
{
	for (auto& l : m_layerStack)
		l->OnAttach();
	
	glfwShowWindow(m_Window.handle);
	while (!glfwWindowShouldClose(m_Window.handle))
	{
		glfwPollEvents();

		{
			static float ct, lt;
			
			ct = (float)glfwGetTime();
			s_DeltaTime = ct - lt;
			lt = ct;
		}

		for (auto& l : m_layerStack)
		{
			l->OnUpdate(s_DeltaTime);
		}
	}
	
	for (auto& l : m_layerStack)
	{
		l->OnDetach();
		PopLayer(l);
		delete l;
	}
}

void Nexus::Application::Shut()
{
	// Modules
	{
		RenderEngine::Shutdown();
	}

	// Window Destruction
	{
		glfwDestroyWindow(m_Window.handle);
		NEXUS_LOG_TRACE("Window Destroyed");
	}
}

void Nexus::Application::SetWindowTitle(const char* name)
{
	glfwSetWindowTitle(m_Window.handle, name);
}

void Nexus::Application::ResizeCallback()
{
	for (auto& layer : m_layerStack)
	{
		layer->OnWindowResize(m_Window.width, m_Window.height);
	}
}

void Nexus::Application::PushLayer(Layer* layer)
{
	m_layerStack.push_back(layer);
}

void Nexus::Application::PopLayer(Layer* layer)
{
	auto it = std::find(m_layerStack.begin(), m_layerStack.end(), layer);
	if (it != m_layerStack.end())
	{
		m_layerStack.erase(it);
	}
}
