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

	auto& materialView = m_Data->PerMaterialHeap;

	for (auto& [k, m] : materialView)
	{
		m_Data->shader->BindShaderResourceHeap(m);

		for (auto& e : meshView)
		{
			entity = Entity(e, m_Scene.get());

			auto& meshc = entity.GetComponent<Component::Mesh>();
			if (meshc.handle == NullUUID)
				continue;

			auto mesh = AssetManager::Get<StaticMesh>(meshc.handle);
			auto& Identity = entity.GetComponent<Component::Identity>();
			m_Data->shader->BindShaderResourceHeap(m_Data->PerEntityHeap[Identity.uuid]);

			for (auto& submesh : mesh->GetSubMeshes())
			{
				if (submesh.material == k)
				{
					Renderer::DrawSubMesh(&submesh);
				}
			}
		}
	}

}
