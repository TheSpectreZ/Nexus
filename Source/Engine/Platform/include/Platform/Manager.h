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
			static void Create_Window(Window* window,const char* title);
			static void Destroy_Window(Window* window);
			
			static void Close(Window* window);
			static bool IsOpen(Window* window);

			static void PollEvents();
		};
	}
}
