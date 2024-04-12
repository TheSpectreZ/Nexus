#pragma once
#include "NxGameEngine/Layer.h"

namespace Nexus
{
	class EditorLayer : public ILayer
	{
	public:
		~EditorLayer() override {};

		void Create() override;
		void Update(float dt) override;
		void Destroy() override;
		
		bool OnWindowResize(int width, int height) override;
		bool OnKeyPress(Key::Code key, InputAction action, InputModifier mods);
		bool OnMousePress(Mouse::Code mouse, InputAction action, InputModifier mods);
	};
}
