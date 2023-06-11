// STL
#include <vector>
// Core
#include "NxCore/Base.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"
// Application
#include "NxApplication/Application.h"
#include "NxApplication/Input.h"
#include "NxApplication/FileDialog.h"
// Modules
#include "NxCore/Input.h"
#include "NxRenderer/Renderer.h"

namespace Nexus
{
	Application* Application::s_Instance = nullptr;	

	struct ApplicationData
	{
		std::vector<Layer*> layerStack;

		const char* clsName = "NEXUS_WINDOW";
		bool IsRunning = true;
		MSG msg;
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
	m_Window.hwnd = nullptr;
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
		
		s_Data->hInst = GetModuleHandle(NULL);

		WNDCLASSEX wc{};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = s_Data->hInst;
		wc.lpszClassName = s_Data->clsName;
		
		RegisterClassEx(&wc);

		RECT wr = { 0, 0, (LONG)m_Window.width, (LONG)m_Window.height };   
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    

		HWND hwnd = CreateWindowEx(0, s_Data->clsName, m_Window.title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL,
			s_Data->hInst, NULL);

		m_Window.hwnd = (void*)hwnd;
		NEXUS_LOG("Application", "Window Created: %i,%i", m_Window.width, m_Window.height);
	}

	// Modules
	{
		Module::RendererCreateInfo rCreateInfo{};
		rCreateInfo.apiType = m_AppSpecs.rApi;
		rCreateInfo.window = &m_Window;
		rCreateInfo.HInstance = s_Data->hInst;
		rCreateInfo.resizeCallback = NEXUS_BIND_FN(Application::ResizeCallback, this);

		Module::Input::Initialize(m_Window);
		Module::Renderer::Initialize(rCreateInfo);
	}
}

void Nexus::Application::Run()
{
	for (auto& l : s_Data->layerStack)
		l->OnAttach();

	ShowWindow((HWND)m_Window.hwnd, SW_SHOW);
	while (s_Data->IsRunning)
	{
		if(PeekMessage(&s_Data->msg, NULL, 0, 0, PM_REMOVE))
		{
			if (Module::Input::Get()->IsKeyPressed(VK_ESCAPE))
				s_Data->msg.message = WM_CLOSE;

			TranslateMessage(&s_Data->msg);
			DispatchMessage(&s_Data->msg);
		
			if (s_Data->msg.message == WM_QUIT)
				s_Data->IsRunning = false;
		}

		for (auto& l : s_Data->layerStack)
			l->OnUpdate(0.f);

		{
			Module::Renderer::Get()->Flush();

			for (auto& l : s_Data->layerStack)
				l->OnRender();

			Module::Renderer::Get()->Flush();
		}
	}

	for (auto& l : s_Data->layerStack)
	{
		l->OnDetach();
		PopLayer(l);
	}
}

void Nexus::Application::Shut()
{
	// Modules
	{
		Module::Renderer::Shutdown();
	}

	DestroyWindow((HWND)m_Window.hwnd);
	UnregisterClass(s_Data->clsName,s_Data->hInst);

	LogManager::Shutdown();
}

void Nexus::Application::SetWindowTitle(const char* name)
{
	SetWindowText((HWND)m_Window.hwnd, name);
}

void Nexus::Application::ResizeCallback()
{
	
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

LRESULT CALLBACK Nexus::Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_SIZE:
		{
			s_Instance->m_Window.width = LOWORD(lParam);
			s_Instance->m_Window.height = HIWORD(lParam);
			break;
		}
		case WM_KEYDOWN:
		{
			Module::Input::Get()->SetKeyState(wParam, true);
			break;
		}
		case WM_KEYUP:
		{
			Module::Input::Get()->SetKeyState(wParam, false);
			break;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}