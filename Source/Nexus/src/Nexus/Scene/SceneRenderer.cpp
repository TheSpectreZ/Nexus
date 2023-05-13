#include "nxpch.h"
#include "SceneRenderer.h"
#include "Renderer/Renderer.h"
#include "Entity.h"
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
	auto meshView = m_Scene->GetAllEntitiesWith<Component::Mesh>();
	for (auto& e : meshView)
	{
		entity = Entity(e, m_Scene.get());

		auto& meshHandle = entity.GetComponent<Component::Mesh>().handle;
		if (meshHandle == NullUUID)
			continue;

		auto& Identity = entity.GetComponent<Component::Identity>();
		auto& resourceHeap = m_Data->PerEntityHeap[Identity.uuid];
		m_Data->shader->BindShaderResourceHeap(resourceHeap);

		auto& meshAsset = AssetManager::Get<StaticMeshAsset>(meshHandle);
		Renderer::DrawMesh(meshAsset.Mesh);
	}
}
