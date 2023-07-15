#pragma once
#include <string>
#include "NxCore/Window.h"

#ifdef NEXUS_ENGINE_SHARED_BUILD
#define NEXUS_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_ENGINE_API __declspec(dllimport)
#endif // NEXUS_ENGINE_SHARED_BUILD

namespace Nexus
{
	namespace FileDialog
	{
		void NEXUS_ENGINE_API SetContextWindow(const Window& window);
		std::string NEXUS_ENGINE_API OpenFile(const char* Filter);
		std::string NEXUS_ENGINE_API SaveFile(const char* Filter);
		std::string NEXUS_ENGINE_API SelectFolder();
	};
}