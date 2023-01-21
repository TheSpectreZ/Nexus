#pragma once
#include "EngineSpecification.h"

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Engine
		{
		public:
			static void Initialize(const EngineSpecification& specs);
			static void Render();
			static void Shutdown();
		};
	}
}