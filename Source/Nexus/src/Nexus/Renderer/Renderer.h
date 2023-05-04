#pragma once
#include "RenderAPI.h"

#include "Context.h"
#include "Swapchain.h"
#include "Command.h"
#include "RenderCommandQueue.h"
#include "TransferCommandQueue.h"

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

		static void BeginRenderCommandQueue();
		static void EndRenderCommandQueue();
		static void FlushRenderCommandQueue();
		
		static void FlushTransferCommandQueue();

		static void WaitForDevice();

		static std::function<void()> ResizeCallback;

		static Ref<Context> GetContext() { return s_Renderer->m_Context; }
		static Ref<Swapchain> GetSwapchain() { return s_Renderer->m_Swapchain; }
		static Ref<RenderCommandQueue> GetRenderCommandQueue() { return s_Renderer->m_RenderCommandQueue; }
		static Ref<TransferCommandQueue> GetTransferCommandQueue() { return s_Renderer->m_TransferCommandQueue; }
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
		Ref<Command> m_RenderCommand;
		Ref<RenderCommandQueue> m_RenderCommandQueue;
		Ref<TransferCommandQueue> m_TransferCommandQueue;
	};
}
