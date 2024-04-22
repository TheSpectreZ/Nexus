#pragma once
#include "NxCore/Event.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif


namespace Nexus
{
	enum class Resolution
	{
		e4K		= 0,	// 3840 x 2160	
		e1440	= 1,	// 2560 x 1440
		e1080	= 2,	// 1920 x 1080
		e720	= 3		// 1280 x 720
	};
	static const char* sResolutionString[] = { "4K","1440p","1080p","720p" };

	struct NEXUS_RENDERER_API ResolutionChangeEvent : public Event
	{
		ResolutionChangeEvent(Resolution res);
		uint32_t width, height;
	};
}
