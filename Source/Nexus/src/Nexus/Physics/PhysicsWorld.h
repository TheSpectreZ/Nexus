#pragma once
#include "Scene/Scene.h"

namespace Nexus
{
	class PhysicsWorld
	{
	public:
		static Ref<PhysicsWorld> Create();

		PhysicsWorld() = default;
		virtual ~PhysicsWorld() = default;

		virtual void OnSceneStart(Ref<Scene> scene) = 0;
		virtual void OnSceneUpdate(float dt) = 0;
		virtual void OnSceneStop() = 0;
	};
}
