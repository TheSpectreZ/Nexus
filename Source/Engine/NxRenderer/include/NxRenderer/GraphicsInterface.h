#pragma once
#include "NxCore/Base.h"
#include "NxCore/Window.h"
#include "RendererAPI.h"

#include "NxGraphics/Context.h"
#include "NxGraphics/Swapchain.h"
#include "NxGraphics/CommandQueue.h"
#include "NxGraphics/Renderpass.h"
#include "NxGraphics/Framebuffer.h"
#include "NxGraphics/Pipeline.h"

namespace Nexus::GraphicsInterface
{
	Ref<Context> CreateContext(RendererAPI API, const ContextCreateInfo& Info);
	Ref<Swapchain> CreateSwapchain(Window* window);
	Ref<CommandQueue> CreateCommandQueue(std::function<void()> resizeCallback);

	Ref<Shader> CreateShader(const ShaderSpecification& specs);
	Ref<Renderpass> CreateRenderpass(const RenderpassSpecification& specs);
	Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& specs);
	Ref<Pipeline> CreatePipeline(const PipelineSpecification& specs);
}
