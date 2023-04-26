#pragma once
#include "SceneBuilder.h"

namespace Nexus
{
	class SceneRenderer
	{
	public:
		SceneRenderer() = default;

		void SetContext(Ref<Scene> scene, Ref<SceneBuildData> data);
		void Render();
	private:
		Ref<Scene> m_Scene = nullptr;
		Ref<SceneBuildData> m_Data = nullptr;
	};
}