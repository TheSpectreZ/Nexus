#pragma once
#include "GraphicsInterface.h"

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
	};

	class NEXUS_RENDERER_API Renderer
	{
		static Renderer* s_Instance;
	public:
		static Renderer* Get() { return s_Instance; }
		static void Initialize(const RendererCreateInfo& Info);
		static void Shutdown();

		Renderer() = default;
		~Renderer() = default;
	private:
		Ref<Context> m_Context;
		Ref<Swapchain> m_Swapchain;
	};
}
