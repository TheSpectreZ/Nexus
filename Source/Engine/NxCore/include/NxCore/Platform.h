#pragma once
#include <string>

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else 
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_CORE_SHARED_BUILD

namespace Nexus::Platform
{
	NEXUS_CORE_API std::string SelectFolder();
	NEXUS_CORE_API std::string OpenFile(const char* Filter);
	NEXUS_CORE_API std::string SaveFile(const char* Filter);

	struct LibraryHANDLE;

	NEXUS_CORE_API LibraryHANDLE* LoadSharedLibrary(const std::string& path);
	NEXUS_CORE_API void UnloadSharedLibrary(LibraryHANDLE* pHandle, const std::string& path);

	NEXUS_CORE_API void* LoadFuncFromSharedLibrary(LibraryHANDLE* pHandle, const char* funcName);
	
	template<typename T>
	T LoadFunc(LibraryHANDLE* pHandle, const char* funcName)
	{
		void* func = LoadFuncFromSharedLibrary(pHandle, funcName);
		return reinterpret_cast<T>(func);
	}

	NEXUS_CORE_API void LaunchProcess(const std::string& process);
}
