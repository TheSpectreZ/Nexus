#pragma once
#include "EngineSpecification.h"
#include <functional>

typedef struct VkCommandBuffer_T* VkCommandBuffer;

#define NX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Engine
		{
		public:
			static void Initialize(const EngineSpecification& specs);
			static void Shutdown();
		};
	}
}