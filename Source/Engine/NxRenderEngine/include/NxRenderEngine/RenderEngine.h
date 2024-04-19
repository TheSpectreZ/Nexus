#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/GraphicsDevice.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus
{
	struct RenderEngineSpecifications
	{
		GraphicsAPI	eAPI;
		Window* pWindow;
		void* hInstance;
	};

	class NEXUS_RENDERER_API RenderEngine
	{
	public:
		bool Initialize(const RenderEngineSpecifications& specs);
		void Terminate();

		void Flush();
		void WaitForGPU();

		void OnScreenResize(int width, int height);
	public:
		GraphicsDevice* getGraphicsDevice() { return mDevice; }
	private:
		struct INTERNAL;
		INTERNAL* mInternal;

		GraphicsDevice* mDevice;
	};
}
