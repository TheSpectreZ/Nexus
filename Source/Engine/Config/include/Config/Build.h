#pragma once
#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_SERIALIZER_DLL

#define NEXUS_SERIALIZER_API __declspec(dllexport)

#else

#define NEXUS_SERIALIZER_API __declspec(dllimport)

#endif // NEXUS_SERIALIZER_DLL


#endif // NEXUS_SYSTEM_WINDOWS
