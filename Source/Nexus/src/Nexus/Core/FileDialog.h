#pragma once
#include <string>
#include "Window.h"

// Currently Supports Windows Only !

namespace Nexus::Platform
{
	namespace FileDialog
	{
		void SetContextWindow(const Window& window);
		std::string OpenFile(const char* Filter);
		std::string SaveFile(const char* Filter);
		std::string SelectFolder();
	};

}