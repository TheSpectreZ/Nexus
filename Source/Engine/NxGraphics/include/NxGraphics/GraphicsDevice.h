#pragma once
#include "NxCore/Window.h"

#include "NxGraphics/Framebuffer.h"
#include "NxGraphics/Shader.h"
#include "NxGraphics/UserInterface.h"

namespace Nexus
{
	enum class GraphicsAPI
	{
		eVulkan
	};

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
		virtual void EndFrame(Framebuffer* framebuffer, uint32_t attachmentIndex) = 0;
		virtual void Flush() = 0;

		virtual void OnScreenResize(int width, int height) = 0;
	public:
		virtual Extent getDeviceExtent() = 0;
	public:
		virtual std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferSpecification& specs) = 0;
		virtual std::shared_ptr<ShaderObject> CreateShaderObject(const ShaderSpecification& specs) = 0;
		virtual std::shared_ptr<UserInterface> CreateUserInterface() = 0;
	};
}