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

		virtual void BeginSwapchainPass() = 0;
		virtual void BeginImGuiPass() = 0;
		virtual void EndPass() = 0;

		virtual Extent GetExtent() = 0;
	};
}