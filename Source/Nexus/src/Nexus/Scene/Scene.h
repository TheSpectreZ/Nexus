#pragma once
#include "entt/entt.hpp"
#include "Component.h"

namespace Nexus
{
	class Entity;
	class Shader;
	
	class Scene
	{
		friend class Entity;
		friend class SceneBuildData;
		friend class SceneRenderer;
		friend class ScriptEngine;
	public:
		static Ref<Scene> Create();
		void Clear();

		Entity CreateEntity();
		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		template<typename... T>
		auto GetAllEntitiesWith()
		{
			return m_registry.view<T...>();
		}

		Entity GetEntityWithUUID(UUID id);

	private:
		entt::registry m_registry;
		std::unordered_map<UUID, Entity> m_EntityMap;
	};
}