#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"
#include "NxScene/Scene.h"
#include "NxScene/Entity.h"

void Nexus::Scene::Clear()
{
	m_registry.each([&](entt::entity e)
		{
			DestroyEntity({ e,this });
		});
	
	m_registry.clear();
	m_EntityMap.clear();
	NEXUS_LOG("ECS", "Scene Cleared");
}

Nexus::Ref<Nexus::Scene> Nexus::Scene::Create()
{
	NEXUS_LOG("ECS", "Scene Created");
	
	auto scene = CreateRef<Scene>();
	return scene;
}

#define COPY_ENTITY_COMPONENT(en,newEn,Name) if(en.HasComponent<Name>()) newEn.AddComponent<Name>(en.GetComponent<Name>())

Nexus::Ref<Nexus::Scene> Nexus::Scene::Duplicate()
{
	Ref<Scene> newScene = CreateRef<Scene>();
	
	Entity entity, newEntity;
	auto entities = m_registry.view<Component::Identity>();
	for (auto& e : entities)
	{
		entity = { e,this };
		newEntity = { newScene->m_registry.create(),newScene.get() };

		auto& i = newEntity.AddComponent<Component::Identity>(entity.GetComponent<Component::Identity>());
		newScene->m_EntityMap[i.uuid] = newEntity;

		newEntity.AddComponent<Component::Tag>(entity.GetComponent<Component::Tag>());
		newEntity.AddComponent<Component::Transform>(entity.GetComponent<Component::Transform>());

		COPY_ENTITY_COMPONENT(entity, newEntity, Component::Mesh);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::Script);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::BoxCollider);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::SphereCollider);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::CapsuleCollider);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::CylinderCollider);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::RigidBody);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::DirectionalLight);
		COPY_ENTITY_COMPONENT(entity, newEntity, Component::PointLight);
	}
	return newScene;
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
	
	return m_EntityMap[i.uuid];
}

Nexus::Entity Nexus::Scene::CreateEntity(const std::string& name, UUID uuid)
{
	entt::entity entity = m_registry.create();

	m_registry.emplace<Component::Tag>(entity, name);
	m_registry.emplace<Component::Identity>(entity, uuid);
	m_registry.emplace<Component::Transform>(entity);
 
	m_EntityMap[uuid] = { entity,this };
	
	return m_EntityMap[uuid];
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	m_EntityMap.erase(entity.GetComponent<Component::Identity>().uuid);
	m_registry.destroy(entity);
}

Nexus::Entity Nexus::Scene::DuplicateEntity(Entity entity)
{
	Entity newEntity = { m_registry.create(),this };

	auto& i = newEntity.AddComponent<Component::Identity>();
	m_EntityMap[i.uuid] = newEntity;

	newEntity.AddComponent<Component::Tag>(entity.GetComponent<Component::Tag>());
	newEntity.AddComponent<Component::Transform>(entity.GetComponent<Component::Transform>());

	COPY_ENTITY_COMPONENT(entity, newEntity, Component::Mesh);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::Script);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::BoxCollider);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::SphereCollider);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::CapsuleCollider);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::CylinderCollider);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::RigidBody);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::DirectionalLight);
	COPY_ENTITY_COMPONENT(entity, newEntity, Component::PointLight);

	return newEntity;
}

Nexus::Entity Nexus::Scene::GetEntityWithUUID(UUID id)
{
	auto end = m_EntityMap.end();
	auto it = m_EntityMap.find(id);

	NEXUS_BREAK_ASSERT((it == end), "Failed To Get Entity With ID");

	return m_EntityMap.at(id);
}
