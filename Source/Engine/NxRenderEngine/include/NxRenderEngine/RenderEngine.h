#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/GraphicsDevice.h"

#include "SceneDrawer.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus
{
	enum class RenderEngineAPI
	{
		Vulkan
	};

	struct RenderEngineSpecifications
	{
		RenderEngineAPI	eAPI;
		Window* pWindow;
		void* hInstance;
	};

	class NEXUS_RENDERER_API RenderEngine
	{
	public:
		bool Intialize(const RenderEngineSpecifications& specs);
		void Terminate();

		void Flush();
		void WaitForGPU();

		void OnScreenResize(int width, int height);
	private:
		struct INTERNAL;
		INTERNAL* mInternal;

		GraphicsDevice* mGraphicsDevice;

		SceneDrawer mSceneDrawer;
	};
}
