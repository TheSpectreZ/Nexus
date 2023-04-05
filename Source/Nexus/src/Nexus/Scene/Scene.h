#pragma once
#include "entt/entt.hpp"

namespace Nexus
{
	class Entity;
	class Shader;
	
	namespace Component
	{
		struct Identity;
	}

	class Scene
	{
		friend class Entity;
	public:
		static Ref<Scene> Create();

		void clear();
		entt::registry& getRegistry() { return m_registry; }

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

		void PushShader(Shader* shader);
		uint32_t PushEntityDeletionCallback(const std::function<void(const Component::Identity&)>& EntityDeletionCallback);
		void PopEntityDeletionCallback(uint32_t Id);
	private:
		entt::registry m_registry;
		std::unordered_map<uint32_t, std::function<void(const Component::Identity&)>> m_EntityDeletionCallback;
		std::vector<Shader*> m_Shaders;
	};
}