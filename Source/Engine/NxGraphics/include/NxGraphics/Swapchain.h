#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	class Swapchain
	{
	public:
		virtual ~Swapchain() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual Extent GetExtent() = 0;
		virtual uint32_t GetImageCount() = 0;
	};
}