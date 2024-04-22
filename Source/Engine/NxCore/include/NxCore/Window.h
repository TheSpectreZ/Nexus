#pragma once
#include <functional>
#include "NxCore/KeyCode.h"
#include "NxCore/Event.h"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	struct WindowResizeEvent : public Event
	{
		WindowResizeEvent(int width, int height)
			:width(width), height(height)
		{}

		int width = 0, height = 0;
	};

	struct KeyPressEvent : public Event
	{
		KeyPressEvent(Key::Code key, InputAction action, InputModifier mods)
			:key(key), action(action), mods(mods)
		{}

		Key::Code key;
		InputAction action;
		InputModifier mods;
	};

	struct MousePressEvent : public Event
	{
		MousePressEvent(Mouse::Code mouse, InputAction action, InputModifier mods)
			:mouse(mouse), action(action), mods(mods)
		{}

		Mouse::Code mouse;
		InputAction action;
		InputModifier mods;
	};
	
	struct Window
	{
		int width = 0, height = 0;
		const char* title = nullptr;
		void* glfwHandle = nullptr;
		void* nativeHandle = nullptr;

		EventDispatcher* eventDispatcher;
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