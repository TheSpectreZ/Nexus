#pragma once
#include "RenderTypes.h"

namespace Nexus
{
	class Swapchain
	{
	public:
		static Ref<Swapchain> Create();

		Swapchain() = default;
		~Swapchain() = default;

		virtual void Init() = 0;
		virtual void Shut() = 0;

		virtual Extent GetExtent() = 0;
		virtual uint32_t GetImageCount() = 0;
	};
}