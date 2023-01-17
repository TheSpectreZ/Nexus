#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_PLATFORM_DLL

#define NEXUS_PLATFORM_API __declspec(dllexport)

#else

#define NEXUS_PLATFORM_API __declspec(dllimport)

#endif

#endif