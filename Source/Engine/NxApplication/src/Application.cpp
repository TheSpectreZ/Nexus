// STL
#include <vector>
#include <chrono>
// GLFW
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
// Core
#include "NxCore/Base.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"
// Application
#include "NxApplication/Application.h"
#include "NxApplication/FileDialog.h"
// Modules
#include "NxCore/Input.h"
#include "NxAsset/Manager.h"
#include "NxRenderEngine/Renderer.h"
#include "NxScriptEngine/ScriptEngine.h"
#include "NxPhysicsEngine/PhysicsEngine.h"

namespace Nexus
{
	Application* Application::s_Instance = nullptr;	

	struct ApplicationData
	{
		std::vector<Layer*> layerStack;
		HINSTANCE hInst;
	};
	
	static ApplicationData* s_Data;
}

Nexus::Application::Application()
{
	s_Instance = this;
	s_Data = new ApplicationData();

	m_Window.title = "Nexus";
	m_Window.width = 600;
	m_Window.height = 400;
	m_Window.glfwHandle = nullptr;
	m_Window.nativeHandle = nullptr;
}

Nexus::Application::~Application()
{
	s_Instance = nullptr;
	delete s_Data;
}

void Nexus::Application::Init()
{
	LogManager::Initialize();
	
	LogManager::Get()->Make(LoggerType::Console);
	LogManager::Get()->Make(LoggerType::File);
	
	// Window Creation
	{
		m_Window.width = m_AppSpecs.Window_Width;
		m_Window.height = m_AppSpecs.Window_height;
		m_Window.title = m_AppSpecs.Window_Title;
		
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		m_Window.glfwHandle = glfwCreateWindow(m_Window.width, m_Window.height, m_Window.title, nullptr, nullptr);

		s_Data->hInst = GetModuleHandle(NULL);
		m_Window.nativeHandle = glfwGetWin32Window((GLFWwindow*)m_Window.glfwHandle);

		// Callbacks
		{
			glfwSetWindowUserPointer((GLFWwindow*)m_Window.glfwHandle, &m_Window);

			glfwSetWindowSizeCallback((GLFWwindow*)m_Window.glfwHandle, [](GLFWwindow* window, int width, int height)
				{
					int w = 0, h = 0;
					glfwGetFramebufferSize(window, &w, &h);
					while (w == 0 || h == 0) {
						glfwGetFramebufferSize(window, &w, &h);
						glfwWaitEvents();
					}

					Window& data = *(Window*)glfwGetWindowUserPointer(window);
					data.width = width;
					data.height = height;
				});
		}

		NEXUS_LOG("Application", "Window Created: %i,%i", m_Window.width, m_Window.height);
	}

	// Modules
	{
		Module::Input::Initialize(m_Window);
		
		Module::RendererCreateInfo rCreateInfo{};
		rCreateInfo.apiType = m_AppSpecs.rApi;
		rCreateInfo.window = &m_Window;
		rCreateInfo.HInstance = s_Data->hInst;
		rCreateInfo.resizeCallback = NEXUS_BIND_FN(Application::ResizeCallback, this);
		rCreateInfo.initSubmodules = m_AppSpecs.EnableRendererSubmodules;
		
		Module::Renderer::Initialize(rCreateInfo);	

		if (m_AppSpecs.EnableAssetManager)
			AssetManager::Initialize();
		
		if (m_AppSpecs.EnablePhysicsEngine)
			PhysicsEngine::Initialize();

		if (m_AppSpecs.EnableScriptEngine)
		{
			ScriptEngineSpecification sCreateInfo{};
			sCreateInfo._MainThreadQueuePtr = &m_MainThreadQueue;

			ScriptEngine::Initialize(sCreateInfo);
		}
	}
}

void Nexus::Application::Run()
{
	static float currentTime = 0.f;
	static float lastTime = 0.f;
	static float deltaTime = 0.f;

	for (auto& l : s_Data->layerStack)
		l->OnAttach();

	glfwShowWindow((GLFWwindow*)m_Window.glfwHandle);
	while (!glfwWindowShouldClose((GLFWwindow*)m_Window.glfwHandle))
	{
		glfwPollEvents();
	
		// Delta Time
		{
			currentTime = (float)glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;
		}
		
		for (auto& l : s_Data->layerStack)
			l->OnUpdate(deltaTime);
		Module::Renderer::Get()->FlushTransfer();

		Module::Renderer::Get()->Begin();
		for (auto& l : s_Data->layerStack)
			l->OnRender();
		Module::Renderer::Get()->End();

		Module::Renderer::Get()->FlushRender();
	}

	Module::Renderer::Get()->WaitForRenderer();

	for (auto& l : s_Data->layerStack)
	{
		l->OnDetach();
		PopLayer(l);
	}
}

void Nexus::Application::Shut()
{
	// Modules [ To-Do ] Clean up this If-Statements
	{
		if (m_AppSpecs.EnableScriptEngine)
			ScriptEngine::Shutdown();

		if (m_AppSpecs.EnablePhysicsEngine)
			PhysicsEngine::Shutdown();

		if (m_AppSpecs.EnableAssetManager)
			AssetManager::Shutdown();
		
		Module::Renderer::Shutdown();
		Module::Input::Shutdown();
	}

	glfwDestroyWindow((GLFWwindow*)m_Window.glfwHandle);
	glfwTerminate();
	
	LogManager::Shutdown();
}

void Nexus::Application::SetWindowTitle(const char* name)
{
	m_Window.title = name;
	glfwSetWindowTitle((GLFWwindow*)m_Window.glfwHandle, name);
}

void Nexus::Application::ResizeCallback()
{
	for (auto& l : s_Data->layerStack)
	{
		l->OnWindowResize(m_Window.width, m_Window.height);
	}
}

void Nexus::Application::PushLayer(Layer* layer)
{
	s_Data->layerStack.push_back(layer);
}

void Nexus::Application::PopLayer(Layer* layer)
{
	auto it = std::find(s_Data->layerStack.begin(), s_Data->layerStack.end(), layer);
	if (it != s_Data->layerStack.end())
	{
		s_Data->layerStack.erase(it);
		delete layer;
	}
}