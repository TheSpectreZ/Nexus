#include "nxpch.h"
#include "GLFW/glfw3.h"

#include "Application.h"
#include "Input.h"

#include "Renderer/Context.h"
#include "Renderer/Renderer.h"

#include "Assets/AssetManager.h"

Nexus::Application* Nexus::Application::s_Instance = nullptr;

static std::vector<Layer*> m_layerStack;

Nexus::Application::Application()
{
	s_Instance = this;

	NEXUS_LOG_INIT

	m_AppSpecs = { 800,600,"Nexus",false,false,RenderAPI_Vulkan };
	m_Window = {};

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
		NEXUS_LOG_TRACE("Window Created: {0}x{1}", m_Window.width, m_Window.height);

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
	}
	
	std::cout << std::endl;

	// Renderer
	{
		RendererSpecifications specs{};
		specs.vsync = m_AppSpecs.Vsync;
		specs.msaa = m_AppSpecs.MultiSampling;

		if (m_AppSpecs.Api == RenderAPI_Vulkan)
			specs.api = RenderAPIType::VULKAN;
		
		Renderer::Init(specs);
		Renderer::ResizeCallback = NEXUS_BIND_EVENT_FN(Application::ResizeCallback);
	}

	AssetManager::Initialize();
}


void Nexus::Application::Run()
{
	std::cout << std::endl;

	{
		for (auto& l : m_layerStack)
			l->OnAttach();
	
		Renderer::FlushTransferCommandQueue();
	}

	glfwShowWindow(m_Window.handle);
	while (!glfwWindowShouldClose(m_Window.handle))
	{
		glfwPollEvents();

		// TimeStep
		{
			static float ct, lt;
			
			ct = (float)glfwGetTime();
			m_TimeStep = Timestep(lt - ct);
			lt = ct;
		}

		// Update
		for (auto& l : m_layerStack)
		{
			l->OnUpdate();
		}
		
		// Swapchain Render Pass and Command Queue
		{
			Renderer::BeginRenderCommandQueue();

			for (auto& l : m_layerStack)
				l->OnRender();

			Renderer::EndRenderCommandQueue();
			Renderer::FlushRenderCommandQueue();
		}
	}
	
	Renderer::WaitForDevice();

	for (auto& l : m_layerStack)
	{
		l->OnDetach();
		PopLayer(l);
		delete l;
	}
	std::cout << std::endl;
}

void Nexus::Application::Shut()
{
	AssetManager::Shutdown();

	Renderer::Shut();
	
	// Window Destruction
	{
		glfwDestroyWindow(m_Window.handle);
		NEXUS_LOG_TRACE("Window Destroyed");
	}
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
