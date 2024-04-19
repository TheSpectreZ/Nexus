#pragma once
#include "NxGameEngine/Layer.h"
#include "NxRenderEngine/SceneDrawer.h"

namespace Nexus
{
	class EditorLayer : public ILayer
	{
	public:
		~EditorLayer() override {};

		void Create() override;
		void Update(float dt) override;
		void Render() override;
		void Destroy() override;
		
		void OnWindowResize(int width, int height) override;
		void OnKeyPress(Key::Code key, InputAction action, InputModifier mods);
		void OnMousePress(Mouse::Code mouse, InputAction action, InputModifier mods);
	private:
		std::shared_ptr<SceneDrawer> mSceneDrawer;
		std::shared_ptr<UserInterface> mUserInterface;

		std::shared_ptr<Framebuffer> mFramebuffer;
	private:
		void RenderImGui();
	};
}
