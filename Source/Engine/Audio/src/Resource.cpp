#include "Audio/Resource.h"
#include "AL/al.h"

#include "minimp3.h"

void Nexus::Audio::Buffer::Create(void* data, int format, int size, int sampleRate)
{
	alGenBuffers(1, &m_buffer);
	alBufferData(m_buffer, format, data, size, sampleRate);
}

void Nexus::Audio::Buffer::Destroy()
{
	alDeleteBuffers(1, &m_buffer);
}

void Nexus::Audio::Source::Create()
{
	alGenSources(1, &m_source);
}

void Nexus::Audio::Source::Play()
{
	alSourcePlay(m_source);
}

void Nexus::Audio::Source::Pause()
{
	alSourcePause(m_source);
}

void Nexus::Audio::Source::Destroy()
{
	alDeleteSources(1, &m_source);
}

void Nexus::Audio::Source::SetPitch(float pitch)
{
	alSourcef(m_source, AL_PITCH, pitch);
}

void Nexus::Audio::Source::SetGain(float gain)
{
	alSourcef(m_source, AL_GAIN, gain);
}

void Nexus::Audio::Source::SetPosition(float pX, float pY, float pZ)
{
	alSource3f(m_source, AL_POSITION, pX, pY, pZ);
}

void Nexus::Audio::Source::SetVelcity(float vX, float vY, float vZ)
{
	alSource3f(m_source, AL_VELOCITY, vX, vY, vZ);
}

void Nexus::Audio::Source::BindBuffer(unsigned int buffer)
{
	alSourcei(m_source, AL_BUFFER, buffer);
}

void Nexus::Audio::Listener::SetPosition(float pX, float pY, float pZ)
{
	float pos[] = { pX,pY,pZ };
	alListenerfv(AL_POSITION, pos);
}

void Nexus::Audio::Listener::SetVelocity(float vX, float vY, float vZ)
{
	float vel[] = { vX,vY,vZ };
	alListenerfv(AL_POSITION, vel);
}

void Nexus::Audio::Listener::SetOrientation(float fX, float fY, float fZ, float uX, float uY, float uZ)
{
	float ori[] = { fX,fY,fZ,uX,uY,uZ };
	alListenerfv(AL_ORIENTATION, ori);
}
