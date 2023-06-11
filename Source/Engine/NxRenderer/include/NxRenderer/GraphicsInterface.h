#pragma once
#include "NxCore/Base.h"
#include "NxCore/Window.h"
#include "RendererAPI.h"

#include "NxGraphics/Context.h"
#include "NxGraphics/Swapchain.h"
#include "NxGraphics/CommandQueue.h"

namespace Nexus::GraphicsInterface
{
	Ref<Context> CreateContext(RendererAPI API, const ContextCreateInfo& Info);
	Ref<Swapchain> CreateSwapchain(Window* window);
	Ref<CommandQueue> CreateCommandQueue(std::function<void()> resizeCallback);
}
