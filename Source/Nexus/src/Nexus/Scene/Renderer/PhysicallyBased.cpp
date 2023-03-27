#include "nxpch.h"
#include "PhysicallyBased.h"

#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Assets/AssetManager.h"
#include "Renderer/Command.h"

void Nexus::SceneRenderer::PhysicallyBased::Setup(Ref<Shader> shader, Ref<Scene> scene)
{
	m_Shader = shader;
	m_Scene = scene;
}

void Nexus::SceneRenderer::PhysicallyBased::Terminate()
{
}

void Nexus::SceneRenderer::PhysicallyBased::Render()
{
	auto view = m_Scene->getRegistry().view<Component::Mesh>();

	for (auto& e : view)
	{
		Entity entity(e, m_Scene.get());

		auto& MeshHandle = entity.GetComponent<Component::Mesh>().handle;
		auto& MeshAsset = AssetManager::Get<StaticMeshAsset>(MeshHandle);
		
		Command::DrawMesh(MeshAsset.Mesh);
	}
}
