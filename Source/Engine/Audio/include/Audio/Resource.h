#pragma once

namespace Nexus
{
	namespace Audio
	{
		class Listener
		{
		public:
			static void SetPosition(float pX,float pY,float pZ);
			static void SetVelocity(float vX, float vY, float vZ);
			static void SetOrientation(float fX, float fY, float fZ, float uX, float uY, float uZ);
		};

		class Buffer
		{
		public:
			void Create(void* data, int format, int size, int sampleRate);
			void Destroy();
		private:
			unsigned int m_buffer;
		};

		class Source
		{
		public:
			void Create();
			void Destroy();
			
			void Play();
			void Pause();

			void SetPitch(float pitch);
			void SetGain(float gain);

			void SetPosition(float pX, float pY, float pZ);
			void SetVelcity(float vX, float vY, float vZ);
			void BindBuffer(unsigned int buffer);
		private:
			unsigned int m_source;
		};

	}
}