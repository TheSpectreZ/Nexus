#pragma once
#include "entt/entt.hpp"
#include "Component.h"

namespace Nexus
{
	class Entity;
	class Shader;
	
	class Scene
	{
	public:
		static Ref<Scene> Create();
		void Clear();

		Entity CreateEntity();
		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, UUID uuid);
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

		std::function<void(Entity)> EntityCreationCallback;
		std::function<void(Entity)> EntityDestructionCallback;
		std::function<void()> SceneDestructionCallback;

		friend class Entity;
		friend class SceneBuildData;
		friend class SceneRenderer;
		friend class SceneSerializer;
		friend class SceneHeirarchy;
		friend class ScriptEngine;
	};
}