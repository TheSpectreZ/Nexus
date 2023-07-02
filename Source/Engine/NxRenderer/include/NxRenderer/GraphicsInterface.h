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
#include "NxGraphics/Buffer.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus::GraphicsInterface
{
	Ref<Context> NEXUS_RENDERER_API CreateContext(RendererAPI API, const ContextCreateInfo& Info);
	Ref<Swapchain> NEXUS_RENDERER_API CreateSwapchain(Window* window);
	Ref<CommandQueue> NEXUS_RENDERER_API CreateCommandQueue(std::function<void()> resizeCallback);

	Ref<Shader> NEXUS_RENDERER_API CreateShader(const ShaderSpecification& specs);
	Ref<Renderpass> NEXUS_RENDERER_API CreateRenderpass(const RenderpassSpecification& specs);
	Ref<Framebuffer> NEXUS_RENDERER_API CreateFramebuffer(const FramebufferSpecification& specs);
	Ref<Pipeline> NEXUS_RENDERER_API CreatePipeline(const PipelineSpecification& specs);
	Ref<Buffer> NEXUS_RENDERER_API CreateBuffer(const BufferSpecification& specs);
}
