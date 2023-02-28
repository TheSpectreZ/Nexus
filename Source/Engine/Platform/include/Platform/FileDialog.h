#pragma once
#include <string>
#include "Window.h"

// Currently Supports Windows Only !

namespace Nexus::Platform
{
	namespace FileDialog
	{
		void NEXUS_PLATFORM_API SetContextWindow(const Window& window);
		std::string NEXUS_PLATFORM_API OpenFile(const char* Filter);
		std::string NEXUS_PLATFORM_API SaveFile(const char* Filter);
		std::string NEXUS_PLATFORM_API SelectFolder();
	};

}