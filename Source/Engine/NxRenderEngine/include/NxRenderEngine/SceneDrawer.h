#pragma once
#include "NxGraphics/GraphicsDevice.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_RENDERER_API SceneDrawer 
	{
	public:
		SceneDrawer(GraphicsDevice* pDevice);
		~SceneDrawer();

		void Draw();
		void OnWindowResize(int width, int height);
	private:
		std::shared_ptr<Framebuffer> mTestFramebuffer;
		std::shared_ptr<ShaderObject> mTestShaderObject;
	};
}
