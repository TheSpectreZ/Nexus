#pragma once
#include "NxGraphics/GraphicsDevice.h"
#include "NxScene/Scene.h"

#include "NxRenderEngine/RenderSettings.h"

namespace Nexus
{
	class NEXUS_RENDERER_API SceneDrawer 
	{
	public:
		SceneDrawer(GraphicsDevice* pDevice, Scene* pScene, Resolution resolution);
		~SceneDrawer();

		void Draw();
		void Resize(uint32_t width, uint32_t height);

		Presentable getPresentable();
	private:
		GraphicsDevice* mDevice;
		Scene* mScene;

		std::shared_ptr<Framebuffer> mTestFramebuffer;
		std::shared_ptr<ShaderObject> mTestShaderObject;

		void CreateSceneResources(int width, int height);
		void DestroySceneResources();
	};
}
