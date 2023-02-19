#pragma once
#include "Build.h"

namespace Nexus
{
	namespace Audio
	{
		class NEXUS_AUDIO_API Engine
		{
		public:
			static Engine Get()
			{
				static Engine Instance;
				return Instance;
			}

			void Initialize();

			void BeginDemo(const char* file);
			void PlayDemo();
			void EndDemo();

			void Shutdown();
		};
	}
}