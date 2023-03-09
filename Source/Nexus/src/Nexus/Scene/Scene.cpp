#include "nxpch.h"
#include "Scene.h"
#include "Entity.h"

void Nexus::Scene::Clear()
{
	m_registry.clear();
}

Nexus::Entity Nexus::Scene::CreateEntity()
{
	entt::entity entity = m_registry.create();
	
	return { entity,this };
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	m_registry.destroy(entity);
}
