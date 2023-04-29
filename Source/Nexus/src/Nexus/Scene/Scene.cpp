#include "nxpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

#include "Assets/AssetManager.h"
#include "Renderer/Shader.h"

void Nexus::Scene::Clear()
{
	m_registry.each([&](entt::entity e)
		{
			DestroyEntity({ e,this });
		});
	
	SceneDestructionCallback();

	m_registry.clear();
	NEXUS_LOG_TRACE("Scene Cleared");
}

Nexus::Ref<Nexus::Scene> Nexus::Scene::Create()
{
	NEXUS_LOG_TRACE("Scene Created");
	return CreateRef<Scene>();
}

Nexus::Entity Nexus::Scene::CreateEntity()
{
	return CreateEntity("Empty Entity");
}

Nexus::Entity Nexus::Scene::CreateEntity(const std::string& name)
{
	entt::entity entity = m_registry.create();
	
	m_registry.emplace<Component::Tag>(entity,name);
	m_registry.emplace<Component::Identity>(entity);
	m_registry.emplace<Component::Transform>(entity);

	return { entity,this };
}

Nexus::Entity Nexus::Scene::CreateEntity(const std::string& name, UUID uuid)
{
	entt::entity entity = m_registry.create();

	m_registry.emplace<Component::Tag>(entity, name);
	m_registry.emplace<Component::Identity>(entity, uuid);
	m_registry.emplace<Component::Transform>(entity);

	return { entity,this };
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	EntityDestructionCallback(entity);
	m_registry.destroy(entity);
}
