#pragma once
#include "RendererUtils.h"

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

		virtual void BeginRenderPass() = 0;
		virtual void EndRenderPass() = 0;

		virtual Extent GetExtent() = 0;
	};
}