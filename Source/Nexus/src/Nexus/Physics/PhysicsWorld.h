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
		
		virtual void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; SetGravityImpl(m_Gravity); }
		virtual glm::vec3 GetGravity() { return m_Gravity; };
	private:
		virtual void SetGravityImpl(const glm::vec3& gravity) = 0;

		glm::vec3 m_Gravity;
	};
}
