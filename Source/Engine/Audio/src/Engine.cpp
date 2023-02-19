#include "Audio/Engine.h"
#include "AL/al.h"
#include "AL/alc.h"

#include "Utility/Assert.h"
#include "Utility/Logger.h"

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"

namespace Nexus
{

	namespace Audio
	{
		static ALCdevice* s_Device = nullptr;
		static ALCcontext* context = nullptr;
	}
	static mp3dec_t s_Mp3d;

	void Audio::Engine::Initialize()
	{
		mp3dec_init(&s_Mp3d);

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

	uint32_t source, buffer;


	void Audio::Engine::BeginDemo(const char* file)
	{
		ALfloat listenerPos[] = { 0.0,0.0,0.0 };
		ALfloat listenerVel[] = { 0.0,0.0,0.0 };
		ALfloat listenerOri[] = { 0.0,0.0,-1.0, 0.0,1.0,0.0 };
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri);

		alGenSources(1, &source);

		alSourcef(source, AL_PITCH, 1);
		alSourcef(source, AL_GAIN, 1);
		alSource3f(source, AL_POSITION, 0, 0, 0);
		alSource3f(source, AL_VELOCITY, 0, 0, 0);
		alSourcei(source, AL_LOOPING, AL_FALSE);

		// Load File
		mp3dec_file_info_t info;
		int res = mp3dec_load(&s_Mp3d, file, &info, nullptr, nullptr);
		uint32_t size = info.samples * sizeof(mp3d_sample_t);

		auto sampleRate = info.hz;
		auto channels = info.channels;

		auto alFormat = 0;

		if (channels == 1)
			alFormat = AL_FORMAT_MONO16;
		else
			alFormat = AL_FORMAT_STEREO16;

		float lengthSeconds = size / (info.avg_bitrate_kbps * 1024.0f);


		alGenBuffers(1, &buffer);
		alBufferData(buffer, alFormat, info.buffer, size, sampleRate);

		alSourcei(source, AL_BUFFER, buffer);
	}

	void Audio::Engine::PlayDemo()
	{
		alSourcePlay(source);
		ALint source_state;
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		// check for errors
		while (source_state == AL_PLAYING) {
			alGetSourcei(source, AL_SOURCE_STATE, &source_state);
			// check for errors
		}
	}

	void Audio::Engine::EndDemo()
	{
		alDeleteSources(1,&source);
		alDeleteBuffers(1,&buffer);
	}

	void Audio::Engine::Shutdown()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(s_Device);
	}

}
