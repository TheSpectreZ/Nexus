#pragma once
#include "RenderAPI.h"

#include "Context.h"
#include "Swapchain.h"
#include "CommandQueue.h"

#include "Mesh.h"
#include "Framebuffer.h"
#include "RenderTypes.h"
#include "Pipeline.h"
#include "Texture.h"

namespace Nexus
{
	struct RendererSpecifications
	{
		RenderAPIType api;
		bool vsync, msaa;
	};

	class Renderer
	{
		static Ref<Renderer> s_Renderer;
		friend class Application;
	public:
		static Ref<Context> GetContext() { return s_Renderer->m_Context; }
		static Ref<Swapchain> GetSwapchain() { return s_Renderer->m_Swapchain; }
		static Ref<CommandQueue> GetCommandQueue() { return s_Renderer->m_CommandQueue; }
		static std::function<void()> ResizeCallback;
		
		static void BeginRenderPass(Ref<Renderpass> pass,Ref<Framebuffer> framebuffer);
		static void EndRenderPass();
		
		static void BindPipeline(Ref<Pipeline> pipeline);
		
		static void SetScissor(Scissor scissor);
		static void SetViewport(Viewport viewport);

		static void DrawMesh(Ref<StaticMesh> mesh);
	private:
		static void Init(const RendererSpecifications& specs);
		static void Shut();

		static void BeginRenderCommandQueue();
		static void EndRenderCommandQueue();
		static void FlushRenderCommandQueue();
		
		static void FlushTransferCommandQueue();

		static void WaitForDevice();
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
		Ref<CommandQueue> m_CommandQueue;
	};
}
