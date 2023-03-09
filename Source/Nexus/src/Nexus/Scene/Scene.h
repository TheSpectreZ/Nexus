#pragma once
#include "entt/entt.hpp"

namespace Nexus
{
	class Entity;

	class Scene
	{
		friend class Entity;
	public:
		void Clear();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);
	private:
		entt::registry m_registry;
	};
}