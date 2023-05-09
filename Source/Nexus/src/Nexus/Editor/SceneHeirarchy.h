#pragma once
#include "Scene/SceneBuilder.h"
#include "Scene/Entity.h"

namespace Nexus
{
	class SceneHeirarchy
	{
	public:
		void SetContext(Ref<SceneBuildData> scenedata, Ref<Scene> scene);
		void Render();
	private:
		Ref<Scene> m_Scene;
		Ref<SceneBuildData> m_SceneData;
		entt::entity m_SelectedEntity;

		void DrawEntityNode(entt::entity e);
		void DrawComponents(entt::entity e);
	};
}