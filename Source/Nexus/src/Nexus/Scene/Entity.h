#pragma once
#include "Scene.h"

namespace Nexus
{
	class Entity
	{
	public:
		Entity();
		Entity(entt::entity entity, Scene* scene);

		template<typename T>
		bool HasComponent()
		{
			return m_scene->m_registry.any_of<T>(m_entity);
		}

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			m_scene->m_registry.emplace<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_scene->m_registry.get<T>(m_entity);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_scene->m_registry.remove<T>(m_entity);
		}

		operator entt::entity() { return m_entity; }
		operator bool() { return m_entity != entt::null; }
		operator uint32_t() { return (uint32_t)m_entity; }

		bool operator==(const Entity& other)
		{
			return m_entity == other.m_entity;
		}

		bool operator!=(const Entity& other)
		{
			return m_entity != other.m_entity;
		}

	private:
		entt::entity m_entity;
		Scene* m_scene;
	};
}
