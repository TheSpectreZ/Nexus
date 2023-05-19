#pragma once
#include "Scene/Scene.h"
#include "Physics/PhysicsWorld.h"

namespace Nexus
{
	class SceneSerializer
	{
	public:
		static void Serialize(Ref<Scene> scene,Ref<PhysicsWorld> physics, const std::string& filepath);
		static bool Deserialize(Scene* scene, PhysicsWorld* physics, const std::string& filepath);
	};
}


