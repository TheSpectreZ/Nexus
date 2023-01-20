#pragma once
#include "Build.h"

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Engine
		{
		public:
			static void Initialize();
			static void Render();
			static void Shutdown();
		};
	}
}