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
	private:
		entt::registry m_registry;

		std::function<void(Entity)> EntityCreationCallback;
		std::function<void(Entity)> EntityDestructionCallback;
		std::function<void()> SceneDestructionCallback;

		friend class Entity;
		friend class SceneBuildData;
		friend class SceneRenderer;
		friend class SceneSerializer;
		friend class SceneHeirarchy;
	};
}