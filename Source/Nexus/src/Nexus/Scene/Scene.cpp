#include "nxpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

#include "Assets/AssetManager.h"
#include "Renderer/Shader.h"

void Nexus::Scene::clear()
{
	for (auto& shader : m_Shaders)
	{
		if (shader == nullptr)
			continue;

		m_registry.each([&](entt::entity e)
			{
				auto& Identity = m_registry.get<Component::Identity>(e);

				shader->DeallocateUniformBuffer(Identity.TransformUniformHandle);
				shader->DeallocateShaderResourceHeap(Identity.ShaderResourceHandle, Identity.SetId);
			});
	}

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
	entt::entity entity = m_registry.create();
	
	m_registry.emplace<Component::Identity>(entity);
	m_registry.emplace<Component::Transform>(entity);

	return { entity,this };
}

void Nexus::Scene::DestroyEntity(Entity entity)
{
	auto& Identity = entity.GetComponent<Component::Identity>();

	for (auto& [k,v] : m_EntityDeletionCallback)
	{
		v(Identity);
	}

	m_registry.destroy(entity);
}

void Nexus::Scene::PushShader(Shader* shader)
{
	m_Shaders.push_back(shader);
}

uint32_t Nexus::Scene::PushEntityDeletionCallback(const std::function<void(const Component::Identity&)>& callback)
{
	static uint32_t Ids = 0;
	m_EntityDeletionCallback[Ids] = callback;
	return Ids++;
}

void Nexus::Scene::PopEntityDeletionCallback(uint32_t Id)
{
	m_EntityDeletionCallback.erase(Id);
}
