#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_USER_INTERFACE_DLL

#define NEXUS_USER_INTERFACE_API __declspec(dllexport)

#else

#define NEXUS_USER_INTERFACE_API __declspec(dllimport)

#endif // NEXUS_USER_INTERFACE_DLL


#endif // NEXUS_SYSTEM_WINDOWS
