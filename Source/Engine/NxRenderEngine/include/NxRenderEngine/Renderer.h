#pragma once
#include <type_traits>
#include <functional>
#include <unordered_map>

#include "GraphicsInterface.h"
#include "ShaderBank.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus
{
	struct NEXUS_RENDERER_API RendererCreateInfo
	{
		RendererAPI apiType;
		Window* window;
		void* HInstance;
		std::function<void()> resizeCallback;

		bool initSubmodules = false;
	};

	class NEXUS_RENDERER_API Renderer
	{
		static Renderer* s_Instance;
	public:
		static void Initialize(const RendererCreateInfo& Info);
		static void Shutdown();

		static void Begin();
		static void End();

		static void FlushTransfer();
		static void FlushRender();

		static void WaitForRenderer();

		static Ref<Context> GetContext() { return s_Instance->m_Context; }
		static Ref<Swapchain> GetSwapchain() { return s_Instance->m_Swapchain; }
		static Ref<CommandQueue> GetCommandQueue() { return s_Instance->m_CommandQueue; }

		Renderer() = default;
		~Renderer() = default;
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
		Ref<CommandQueue> m_CommandQueue;
		Ref<ShaderBank> m_ShaderBank;
	};
}
