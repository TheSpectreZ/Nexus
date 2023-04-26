#include "nxpch.h"
#include "SceneRenderer.h"
#include "Entity.h"
#include "Renderer/Command.h"
#include "Assets/AssetManager.h"

void Nexus::SceneRenderer::SetContext(Ref<Scene> scene, Ref<SceneBuildData> data)
{
	m_Scene = scene;
	m_Data = data;
}

void Nexus::SceneRenderer::Render()
{
	m_Data->shader->BindShaderResourceHeap(m_Data->PerSceneHeap);

	Entity entity;
	auto meshView = m_Scene->m_registry.view<Component::Mesh>();
	for (auto& e : meshView)
	{
		entity = Entity(e, m_Scene.get());

		auto& Identity = entity.GetComponent<Component::Identity>();
		auto& resourceHeap = m_Data->PerEntityHeap[Identity.uuid];

		m_Data->shader->BindShaderResourceHeap(resourceHeap);

		auto& meshHandle = entity.GetComponent<Component::Mesh>().handle;
		auto& meshAsset = AssetManager::Get<StaticMeshAsset>(meshHandle);

		Command::DrawMesh(meshAsset.Mesh);
	}
}
