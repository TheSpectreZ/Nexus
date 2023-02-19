#pragma once
#include "Build.h"
#include "Registry.h"

namespace Nexus::Utility
{
	class Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, entt::registry* registry);

		template<typename T>
		bool HasComponent()
		{
			return m_registry->any_of<T>(m_handle);
		}

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_registry->emplace<T>(m_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_registry->get<T>(m_handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_registry->remove<T>(m_handle);
		}

		operator entt::entity() { return m_handle; }
		operator uint64_t() { return (uint64_t)m_handle; }
		operator bool() { return m_handle != entt::null; }
		bool operator ==(Entity& other) const { return m_handle == other.m_handle; }
		bool operator !=(Entity& other) const { return m_handle != other.m_handle; }
	private:
		entt::entity m_handle;
		entt::registry* m_registry;
	};
}
