#pragma once
#include "RenderAPI.h"

#include "Context.h"
#include "Swapchain.h"
#include "RenderCommand.h"
#include "RenderCommandQueue.h"

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
	public:
		static void Init(const RendererSpecifications& specs);
		static void Shut();

		static void Begin();
		static void End();
		static void Flush();

		static void WaitForDevice();

		static Ref<Context> GetContext() { return s_Renderer->m_Context; }
		static Ref<Swapchain> GetSwapchain() { return s_Renderer->m_Swapchain; }
		static Ref<RenderCommandQueue> GetRenderCommandQueue() { return s_Renderer->m_RenderCommandQueue; }
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
		Ref<RenderCommand> m_RenderCommand;
		Ref<RenderCommandQueue> m_RenderCommandQueue;
	};
}
