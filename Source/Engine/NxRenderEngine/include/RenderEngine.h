#pragma once
#include "RenderAPI.h"

namespace Nexus
{
	class RenderEngine
	{
		static RenderEngine* s_Instance;
	public:
		static void Initialize(RenderAPIType Type);
		static void Shutdown();
	};
}