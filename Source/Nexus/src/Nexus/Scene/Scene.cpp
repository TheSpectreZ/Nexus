#include "nxpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

#include "Assets/AssetManager.h"
#include "Renderer/Command.h"

void Nexus::Scene::clear()
{
	m_registry.clear();
}

Nexus::Ref<Nexus::Scene> Nexus::Scene::Create()
{
	return CreateRef<Scene>();
}

Nexus::Entity Nexus::Scene::CreateEntity()
{
	entt::entity entity = m_registry.create();
	
	m_registry.emplace<Component::Transform>(entity);

	return { entity,this };
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	m_registry.destroy(entity);
}