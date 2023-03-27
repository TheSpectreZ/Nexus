#pragma once
#include "entt/entt.hpp"

namespace Nexus
{
	class Entity;
	
	class Scene
	{
		friend class Entity;
	public:
		static Ref<Scene> Create();

		void clear();
		entt::registry& getRegistry() { return m_registry; }

		Entity CreateEntity();
		void DestroyEntity(Entity entity);
	private:
		entt::registry m_registry;
	};
}