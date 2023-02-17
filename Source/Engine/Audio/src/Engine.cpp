#include "Audio/Engine.h"
#include "AL/al.h"
#include "AL/alc.h"

#include "DebugUtils/Assert.h"
#include "DebugUtils/Logger.h"

namespace Nexus
{

	namespace Audio
	{
		static ALCdevice* s_Device = nullptr;
	}

	void Audio::Engine::Initialize()
	{
		ALCcontext* context;
		
		s_Device = alcOpenDevice(nullptr);
		NEXUS_ASSERT(s_Device == nullptr, "Couldn't Open Audio Device");

		context = alcCreateContext(s_Device, nullptr);

		if (context == nullptr || alcMakeContextCurrent(context) == ALC_FALSE)
		{
			if (context != nullptr)
				alcDestroyContext(context);

			alcCloseDevice(s_Device);
			NEXUS_ASSERT(1, "Couldn't Set Audio Context");
		}
	}

	void Audio::Engine::Shutdown()
	{
		alcCloseDevice(s_Device);
	}

}
