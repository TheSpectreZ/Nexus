#pragma once
#include <type_traits>
#include <functional>
#include <unordered_map>

#include "GraphicsInterface.h"
#include "ResourcePool.h"
#include "RenderableScene.h"	
#include "Drawer.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus::Module
{
	struct NEXUS_RENDERER_API RendererCreateInfo
	{
		RendererAPI apiType;
		Window* window;
		void* HInstance;
		std::function<void()> resizeCallback;
	};

	class NEXUS_RENDERER_API Renderer
	{
		static Renderer* s_Instance;
	public:
		static Renderer* Get() { return s_Instance; }
		
		static void Initialize(const RendererCreateInfo& Info);
		static void Shutdown();

		void Begin();
		void End();

		void FlushTransfer();
		void FlushRender();

		Renderer() = default;
		~Renderer() = default;

		Ref<Context> GetContext() { return m_Context; }
		Ref<Swapchain> GetSwapchain() { return m_Swapchain; }
		Ref<CommandQueue> GetCommandQueue() { return m_CommandQueue; }
		Ref<ResourcePool> GetResourcePool() { return m_ResourcePool; }

		void Submit(Ref<Scene> scene);
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
		Ref<CommandQueue> m_CommandQueue;
		Ref<ResourcePool> m_ResourcePool;

		Ref<ForwardDrawer> m_ForwardDrawer;

		void ResizeCallback();
	};
}
