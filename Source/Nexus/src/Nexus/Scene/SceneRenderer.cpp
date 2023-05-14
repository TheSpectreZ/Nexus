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

	std::unordered_map<UUID, std::vector<std::pair<SubMesh*, Entity>>> meshes;

	for (auto& e : meshView)
	{
		entity = Entity(e, m_Scene.get());

		auto& meshc = entity.GetComponent<Component::Mesh>();
		if (meshc.handle == NullUUID)
			continue;

		auto mesh = AssetManager::Get<StaticMesh>(meshc.handle);
		auto& sb = mesh->GetSubMeshes();

		for (size_t i = 0; i < sb.size(); i++)
		{
			if (sb[i].material == NullUUID)
			{
				NEXUS_LOG_ERROR("Null Material: {0}", i);
				continue;
			}

			meshes[mesh->GetSubMeshes()[i].material].emplace_back(std::make_pair(&sb[i], entity));
		}
	}

	for (auto& [k, v] : meshes)
	{
		m_Data->shader->BindShaderResourceHeap(m_Data->PerMaterialHeap[k]);

		for (auto& m : v)
		{
			auto sb = m.first;
			auto en = m.second;

			auto& Identity = en.GetComponent<Component::Identity>();
			m_Data->shader->BindShaderResourceHeap(m_Data->PerEntityHeap[Identity.uuid]);

			Renderer::DrawSubMesh(sb);
		}
	}
}
