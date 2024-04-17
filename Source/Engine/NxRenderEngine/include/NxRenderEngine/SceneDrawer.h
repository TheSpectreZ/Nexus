#pragma once
//#include "NxScene/Scene.h"
#include "NxGraphics/GraphicsDevice.h"

namespace Nexus
{
	class SceneDrawer
	{
	public:
		void Create(GraphicsDevice* pDevice, const class Scene* pScene);
		void Draw();
		void Destroy();

		Ref<Framebuffer> GetPresentableFramebuffer() { return mTestFramebuffer; }
	private:
		Ref<Framebuffer> mTestFramebuffer;
	};
}
