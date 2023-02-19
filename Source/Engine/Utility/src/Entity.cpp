#include "Utility/Entity.h"

Nexus::Utility::Entity::Entity()
	:m_handle(entt::null),m_registry(nullptr)
{}

Nexus::Utility::Entity::Entity(entt::entity handle, entt::registry* registry)
	:m_handle(handle), m_registry(registry)
{}
