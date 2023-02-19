#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_UTILITY_DLL

#define NEXUS_UTILITY_API __declspec(dllexport)

#else 

#define NEXUS_UTILITY_API __declspec(dllimport)

#endif // NEXUS_DEBUG_DLL


#endif // NEXUS_SYSTEM_WINDOWS
