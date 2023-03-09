#include "nxpch.h"
#include "Entity.h"

Nexus::Entity::Entity()
{
	m_scene = nullptr;
	m_entity = entt::null;
}

Nexus::Entity::Entity(entt::entity entity, Scene* scene)
{
	m_scene = scene;
	m_entity = entity;
}
