#pragma once

#ifdef NEXUS_SYSTEM_WINDOWS

#ifdef NEXUS_AUDIO_DLL

#define NEXUS_AUDIO_API __declspec(dllexport)

#else

#define NEXUS_AUDIO_API __declspec(dllimport)

#endif // NEXUS_AUDIO_DLL

#endif