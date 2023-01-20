#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_GRAPHICS_DLL

#define NEXUS_GRAPHICS_API __declspec(dllexport)

#else 

#define NEXUS_GRAPHICS_API __declspec(dllimport)

#endif // NEXUS_GRAPHICS_DLL

#endif // NEXUS_SYSTEM_WINDOWS
