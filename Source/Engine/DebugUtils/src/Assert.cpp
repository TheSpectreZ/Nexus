#include "DebugUtils/Assert.h"

#include <Windows.h>
#include <sstream>

void NEXUS_DEBUG_API Nexus::DebugUtils::Assert(bool condition, const char* message, const char* file,int line)
{
	if (condition)
	{
		std::stringstream ss;
		ss << message << "\n" << "[LINE]: " << line << "\n" << "[FILE]: " << file << "\n";

		MessageBoxA(NULL, ss.str().c_str(), "Nexus Assert", MB_ICONEXCLAMATION | MB_OK);
		exit(-2);
	}
}
