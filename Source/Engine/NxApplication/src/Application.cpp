// STL
#include <vector>

// Application
#include "Base.h"
#include "Logger.h"
#include "Application.h"
#include "Input.h"
#include "FileDialog.h"

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

		m_Window.handle = CreateWindowEx(0, s_Data->clsName, m_Window.title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL,
			s_Data->hInst, NULL);

		NEXUS_LOG("Application", "Window Created: %i,%i", m_Window.width, m_Window.height);
	}

}

void Nexus::Application::Run()
{
	for (auto& l : s_Data->layerStack)
		l->OnAttach();

	ShowWindow(m_Window.handle, SW_SHOW);
	while (s_Data->IsRunning)
	{
		if(PeekMessage(&s_Data->msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&s_Data->msg);
			DispatchMessage(&s_Data->msg);
		
			if (s_Data->msg.message == WM_QUIT)
				s_Data->IsRunning = false;
		}

		for (auto& l : s_Data->layerStack)
			l->OnUpdate(0.f);

		for (auto& l : s_Data->layerStack)
			l->OnRender();
	}

	for (auto& l : s_Data->layerStack)
	{
		l->OnDetach();
		PopLayer(l);
	}
}

void Nexus::Application::Shut()
{
	UnregisterClass(s_Data->clsName,s_Data->hInst);

	LogManager::Shutdown();
}

void Nexus::Application::SetWindowTitle(const char* name)
{
	SetWindowText(m_Window.handle, name);
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
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
