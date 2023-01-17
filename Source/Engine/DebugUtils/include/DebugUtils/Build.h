#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_DEBUG_DLL

#define NEXUS_DEBUG_API __declspec(dllexport)

#else 

#define NEXUS_DEBUG_API __declspec(dllimport)

#endif // NEXUS_DEBUG_DLL


#endif // NEXUS_SYSTEM_WINDOWS
