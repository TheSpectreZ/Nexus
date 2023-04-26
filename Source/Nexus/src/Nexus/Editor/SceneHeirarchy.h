#pragma once
#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Nexus
{
	class SceneHeirarchy
	{
	public:
		void SetContext(Ref<Scene> scene);
		void Render();
	private:
		Ref<Scene> m_Scene;
		entt::entity m_SelectedEntity;

		void DrawEntityNode(entt::entity e);
		void DrawComponents(entt::entity e);
	};
}