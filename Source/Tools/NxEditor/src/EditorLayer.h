#pragma once
#include "NxGameEngine/Layer.h"
#include "NxRenderEngine/SceneDrawer.h"

#include "Panels/ContentBrowser.h"

namespace Nexus
{
	class EditorLayer : public ILayer
	{
	public:
		EditorLayer();
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

		EventDispatcher mUIeventDispatcher;

		EditorPanel::ContentBrowser  mContentBrowser;
	private:
		void RenderImGUI();
		void UI_RenderSettings();
	};
}
