#include "nxpch.h"

#include <Windows.h>
#include <sstream>

void Nexus::Utility::Assert(const bool condition, const char* message, const char* file,const int line)
{
	if (condition)
	{
		std::stringstream ss;
		ss << message << "\n" << "[LINE]: " << line << "\n" << "[FILE]: " << file << "\n";

		MessageBoxA(nullptr, ss.str().c_str(), "Nexus Assert", MB_ICONEXCLAMATION | MB_OK);
	
		exit(-1);
	}
}
