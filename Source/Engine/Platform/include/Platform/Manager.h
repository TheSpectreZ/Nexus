#pragma once
#include "Window.h"

namespace Nexus
{
	namespace Platform
	{
		void NEXUS_PLATFORM_API Initialize();
		void NEXUS_PLATFORM_API Shutdown();

		class NEXUS_PLATFORM_API Manager
		{
		public:
			static void Create_Window(Window& window);
			static void Destroy_Window(const Window& window);
			
			static void Close(const Window& window);
			static bool IsOpen(const Window& window);

			static void PollEvents();
		
			static float GetDeltaTime() { return deltaTime; }
		private:
			static float deltaTime;
		};
	}
}
