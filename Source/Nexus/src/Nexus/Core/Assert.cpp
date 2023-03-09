#include "nxpch.h"

#include <Windows.h>
#include <sstream>

#ifdef NEXUS_DEBUG
#include "Application.h"
#endif // NEXUS_DEBUG


void Nexus::Utility::Assert(const bool condition, const char* message, const char* file,const int line)
{
	if (condition)
	{
		std::stringstream ss;
		ss << message << "\n" << "[LINE]: " << line << "\n" << "[FILE]: " << file << "\n";

		MessageBoxA(nullptr, ss.str().c_str(), "Nexus Assert", MB_ICONEXCLAMATION | MB_OK);
	
#ifdef NEXUS_ASSERT
		Application::BreakOnAssert();
#endif // NEXUS_ASSERT

		exit(-1);
	}
}
