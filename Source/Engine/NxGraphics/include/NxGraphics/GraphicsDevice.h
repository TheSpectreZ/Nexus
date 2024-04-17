#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/Framebuffer.h"

namespace Nexus
{
	struct GraphicsDeviceSpecifications
	{
		Window* pWindow;
		void* hInstance;
	};

	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() {};

		virtual bool Initialize(const GraphicsDeviceSpecifications& specs) = 0;
		virtual void Shutdown() = 0;
		virtual void WaitForGPU() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) = 0;
		virtual void Flush() = 0;

		virtual void OnScreenResize(int width, int height) = 0;
	public:
		virtual Extent getDeviceExtent() = 0;
	public:
		virtual Ref< Framebuffer > CreateFramebuffer(const FramebufferSpecification& specs) = 0;
	};
}