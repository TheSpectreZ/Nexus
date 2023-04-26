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
	public:
		static Ref<Scene> Create();
		void Clear();

		Entity CreateEntity();
		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);
	private:
		entt::registry m_registry;
	};
}