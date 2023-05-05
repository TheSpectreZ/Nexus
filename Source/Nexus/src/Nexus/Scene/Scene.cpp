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
	m_registry.emplace<Component::Transform>(entity);
  auto& i = m_registry.emplace<Component::Identity>(entity);
  
  m_EntityMap[i.uuid] = { entity,this };
  EntityCreationCallback(m_EntityMap[i.uuid]);
  
	return m_EntityMap[i.uuid];
}

Nexus::Entity Nexus::Scene::CreateEntity(const std::string& name, UUID uuid)
{
	entt::entity entity = m_registry.create();

	m_registry.emplace<Component::Tag>(entity, name);
	m_registry.emplace<Component::Identity>(entity, uuid);
	m_registry.emplace<Component::Transform>(entity);
 
  m_EntityMap[uuid] = { entity,this };
  EntityCreationCallback(m_EntityMap[uuid]);
  
	return m_EntityMap[uuid];
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	EntityDestructionCallback(entity);
	m_registry.destroy(entity);
}

Nexus::Entity Nexus::Scene::GetEntityWithUUID(UUID id)
{
	auto end = m_EntityMap.end();
	auto it = m_EntityMap.find(id);

	NEXUS_ASSERT((it == end), "Failed To Get Entity With ID");

	return m_EntityMap.at(id);
}
