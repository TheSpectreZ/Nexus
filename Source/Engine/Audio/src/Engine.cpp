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

		{
			NEXUS_LOG_WARN("Audio Device Info:");
			NEXUS_LOG_INFO("  Name: " << s_Device->DeviceName);
			NEXUS_LOG_INFO("  Sample Rate: " << s_Device->Frequency);
			NEXUS_LOG_INFO("  Max Sources: " << s_Device->SourcesMax);
			NEXUS_LOG_INFO("    Mono: " << s_Device->NumMonoSources);
			NEXUS_LOG_INFO("    Stereo: " << s_Device->NumStereoSources);
		}
	}

	void Audio::Engine::Shutdown()
	{
		alcCloseDevice(s_Device);
	}

}
