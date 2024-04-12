#pragma once
#include <functional>

#include "NxCore/KeyCode.h"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	struct EventHandler
	{
		std::function<void(int, int)> OnResize;
		std::function<void(Key::Code, InputAction, InputModifier)> OnKeyPress;
		std::function<void(Mouse::Code, InputAction, InputModifier)> OnMousePress;
	};
	
	struct Window
	{
		int width = 0, height = 0;
		const char* title = nullptr;
		void* glfwHandle = nullptr;
		void* nativeHandle = nullptr;

		EventHandler eventHandler;
	};

	namespace Windowing
	{
		void NEXUS_CORE_API Init();
		void NEXUS_CORE_API Shut();

		bool NEXUS_CORE_API Create(Window* pInWindow);
		void NEXUS_CORE_API Destroy(const Window* pWindow);

		void NEXUS_CORE_API Show(Window* pInWindow);
		void NEXUS_CORE_API PollEvents();
		bool NEXUS_CORE_API IsOpen(const Window* pWindow);

		double NEXUS_CORE_API GetDeltaTime();
	}
}