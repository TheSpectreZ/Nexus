#pragma once
#define WIN32_LEAN_AND_MEAN
#include "entt/entt.hpp"
#include "NxCore/Base.h"

#include "Component.h"
#include "Camera.h"

namespace Nexus
{
	class Entity;
	class Shader;
	
	class NEXUS_SCENE_API Scene
	{
	public:
		Scene();
		~Scene();

		UUID GetId() { return m_Id; }

		Ref<Scene> Duplicate();
		
		void SetCamera(Camera* cam) { m_Camera = cam; }
		Camera* GetCamera() { return m_Camera; }

		void Clear();

		Entity CreateEntity();
		Entity CreateEntity(const std::string& name);
		Entity CreateEntity(const std::string& name, UUID uuid);
		void DestroyEntity(Entity entity);

		Entity DuplicateEntity(Entity entity);

		template<typename... T>
		auto GetAllEntitiesWith()
		{
			return m_registry.view<T...>();
		}

		Entity GetEntityWithUUID(UUID id);
	private:
		UUID m_Id;
		Camera* m_Camera;
		entt::registry m_registry;
		std::unordered_map<UUID, Entity> m_EntityMap;

		friend class Entity;
		friend class SceneBuildData;
		friend class SceneRenderer;
		friend class SceneSerializer;
		friend class SceneHeirarchy;
		friend class ScriptEngine;
	};
}