#pragma once
#include "Base.h"
#include "Window.h"
#include "Layer.h"
#include "ApplicationSpecifications.h"
#include "TimeStep.h"
#include "Renderer/Context.h"

#include <mutex>

namespace Nexus
{
	class Application
	{
		static Application* s_Instance;
	public:
		Application();
		~Application();

		void Init();
		void Run();
		void Shut();

		static Application* Get() { return s_Instance; }
		
		Timestep& GetTimeStep() { return m_TimeStep; }
		Window& GetWindow() { return m_Window; }

		void SetWindowTitle(const char* name);

		void SubmitToMainThreadQueue(const std::function<void()> func);
	protected:
		ApplicationSpecifications m_AppSpecs;

		void ResizeCallback();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		Window m_Window;
		Timestep m_TimeStep;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;
	};

}

