#include "nxpch.h"
#include "PhysicallyBased.h"

#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Assets/AssetManager.h"
#include "Renderer/Command.h"

void Nexus::SceneRenderer::PhysicallyBased::Initialize(Ref<Shader> shader, Ref<Scene> scene,Camera* camera)
{
	m_Shader = shader;
	m_Scene = scene;
	m_Camera = camera;

	m_Scene->PushShader(m_Shader.get());
	m_CallbackId = m_Scene->PushEntityDeletionCallback(NEXUS_BIND_EVENT_FN(OnDeletionCallback));

	BuildShaderData();
	NEXUS_LOG_INFO("PBR Initailized");
}

void Nexus::SceneRenderer::PhysicallyBased::Terminate()
{
	m_Scene->PopEntityDeletionCallback(m_CallbackId);
	m_Shader.reset();
	NEXUS_LOG_INFO("PBR Terminated")
}

void Nexus::SceneRenderer::PhysicallyBased::Render()
{
	auto view = m_Scene->getRegistry().view<Component::Mesh>();

	auto cam = m_Camera->projection * m_Camera->view;

	Entity entity; 
	for (auto& e : view)
	{
		entity = Entity(e, m_Scene.get());
		auto& Identity = entity.GetComponent<Component::Identity>();

		auto Transform = cam * entity.GetComponent<Component::Transform>().GetTransform();
		m_Shader->SetUniformData(Identity.TransformUniformHandle, glm::value_ptr(Transform));

		auto& MeshHandle = entity.GetComponent<Component::Mesh>().handle;
		auto& MeshAsset = AssetManager::Get<StaticMeshAsset>(MeshHandle);

		m_Shader->BindShaderResourceHeap(Identity.ShaderResourceHandle, Identity.SetId);

		Command::DrawMesh(MeshAsset.Mesh);
	}
}

void Nexus::SceneRenderer::PhysicallyBased::BuildShaderData()
{
	auto view = m_Scene->getRegistry().view<Component::Transform>();
	Entity entity;
	for (auto& e : view)
	{
		entity = Entity(e, m_Scene.get());
		auto& Identity = entity.GetComponent<Component::Identity>();
		Identity.SetId = 0;

		m_Shader->AllocateShaderResourceHeap(Identity.ShaderResourceHandle, Identity.SetId);
		
		m_Shader->AllocateUniformBuffer(Identity.TransformUniformHandle, Identity.SetId, 0);
		m_Shader->BindUniformWithResourceHeap(Identity.TransformUniformHandle, Identity.ShaderResourceHandle, Identity.SetId, 0);
	}
}

void Nexus::SceneRenderer::PhysicallyBased::OnDeletionCallback(const Component::Identity& Identity)
{
	m_Shader->DeallocateShaderResourceHeap(Identity.ShaderResourceHandle, Identity.SetId);
	m_Shader->DeallocateUniformBuffer(Identity.TransformUniformHandle);
}
