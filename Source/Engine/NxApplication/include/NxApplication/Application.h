#pragma once
#define NOMINMAX
#include <Windows.h>
#include <cstdint>	
// Application
#include "Layer.h"
#include "ApplicationSpecifications.h"
#include "NxCore/Window.h"

#ifdef NEXUS_ENGINE_SHARED_BUILD
#define NEXUS_ENGINE_API __declspec(dllexport)
#else 
#define NEXUS_ENGINE_API __declspec(dllimport)
#endif // NEXUS_ENGINE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_ENGINE_API Application
	{
		static Application* s_Instance;
	public:
		static Application* Get() { return s_Instance; }
		
		Application();
		~Application();

		void Init();
		void Run();
		void Shut();

		void SetWindowTitle(const char* name);
		const Window& GetWindow() { return m_Window; }
		const ApplicationSpecifications& GetAppSpecs() { return m_AppSpecs; }
	protected:
		ApplicationSpecifications m_AppSpecs;

		void ResizeCallback();
		
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		Window m_Window;
	};

}

