#pragma once
#include "Scene/Scene.h"

namespace Nexus
{
	class SceneSerializer
	{
	public:
		static void Serialize(Ref<Scene> scene, const std::string& filepath);
		static bool Deserialize(Ref<Scene> scene, const std::string& filepath);
	};
}


