#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_CORE_DLL

#define NEXUS_CORE_API __declspec(dllexport)

#else

#define NEXUS_CORE_API __declspec(dllimport)

#endif // NEXUS_CORE_DLL


#endif // NEXUS_SYSTEM_WINDOWS
