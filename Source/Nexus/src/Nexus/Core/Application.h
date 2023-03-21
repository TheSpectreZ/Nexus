#pragma once
#include "Base.h"
#include "Window.h"
#include "Layer.h"
#include "ApplicationSpecifications.h"
#include "TimeStep.h"
#include "Renderer/Context.h"

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
		
#ifdef NEXUS_DEBUG
		static void BreakOnAssert();
#endif // NEXUS_DEBUG

		Timestep& GetTimeStep() { return m_TimeStep; }
		Window& GetWindow() { return m_Window; }
	protected:
		ApplicationSpecifications m_AppSpecs;

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		Window m_Window;
		Timestep m_TimeStep;
	};

}

