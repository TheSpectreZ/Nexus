#include "NxRenderer/RenderableScene.h"
#include "NxRenderer/ResourcePool.h"
#include "NxAsset/Manager.h"

Nexus::RenderableScene::RenderableScene(Ref<Scene> scene, Ref<Shader> shader)
	:m_Shader(shader),m_Scene(scene)
{
	Initialize();
}

Nexus::RenderableScene::~RenderableScene()
{
	Destroy();
}

void Nexus::RenderableScene::Prepare()
{
	auto cameraBuf = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform0.hashId);
	cameraBuf->Update(m_Scene->GetCamera());
}

void Nexus::RenderableScene::Draw(Ref<CommandQueue> queue)
{
	queue->BindShaderResourceHeap(m_Shader, PerSceneHeap);

	Entity entity;
	auto view = m_Scene->GetAllEntitiesWith<Component::Mesh>();
	for (auto& e : view)
	{
		entity = { e,m_Scene.get() };

		auto MeshHandle = entity.GetComponent<Component::Mesh>().handle;
		if (!MeshHandle)
			continue;
		
		auto& Identity = entity.GetComponent<Component::Identity>();
		if (!PerEntityHeap.contains(Identity.uuid))
			CreateEntityResource(Identity.uuid);

		auto Transform = entity.GetComponent<Component::Transform>().GetTransform();
		auto buff = ResourcePool::Get()->GetUniformBuffer(PerEntityUniform[Identity.uuid].hashId);
		buff->Update(glm::value_ptr(Transform));
		
		auto RTMesh = ResourcePool::Get()->GetRenderableMesh(MeshHandle);

		queue->BindShaderResourceHeap(m_Shader, PerEntityHeap[Identity.uuid]);
		queue->BindVertexBuffer(RTMesh->GetVertexBuffer());
		queue->BindIndexBuffer(RTMesh->GetIndexBuffer());
		queue->DrawIndices(RTMesh->GetIndexBuffer()->GetSize() / sizeof(uint32_t));
	}
}

void Nexus::RenderableScene::Initialize()
{
	// Scene
	{
		PerSceneHeap.hashId = UUID();
		PerSceneHeap.set = 0;
		m_Shader->AllocateShaderResourceHeap(PerSceneHeap);

		// Camera
		PerSceneUniform0.hashId = UUID();
		PerSceneUniform0.set = 0;
		PerSceneUniform0.binding = 0;
		auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, PerSceneUniform0);

		m_Shader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform0.binding, buff);
	}

	// Entity
	{
		Entity entity;
		auto view = m_Scene->GetAllEntitiesWith<Component::Mesh>();
		for (auto& e : view)
		{
			entity = Entity(e, m_Scene.get());
			auto& Identity = entity.GetComponent<Component::Identity>();

			CreateEntityResource(Identity.uuid);
		}
	}
}

void Nexus::RenderableScene::Destroy()
{
	m_Shader->DeallocateShaderResourceHeap(PerSceneHeap);
	ResourcePool::Get()->DeallocateUniformBuffer(PerSceneUniform0.hashId);
	ResourcePool::Get()->DeallocateUniformBuffer(PerSceneUniform1.hashId);

	for (auto& [k, v] : PerEntityHeap)
	{
		m_Shader->DeallocateShaderResourceHeap(v);
		ResourcePool::Get()->DeallocateUniformBuffer(PerEntityUniform[k].hashId);
	}
	PerEntityHeap.clear();
	PerEntityUniform.clear();
}

void Nexus::RenderableScene::CreateEntityResource(UUID Id)
{
	ResourceHeapHandle heapHandle{};
	heapHandle.hashId = UUID();
	heapHandle.set = 1;

	m_Shader->AllocateShaderResourceHeap(heapHandle);
	PerEntityHeap[Id] = heapHandle;

	UniformBufferHandle uniformHandle{};
	uniformHandle.hashId = UUID();
	uniformHandle.set = 1;
	uniformHandle.binding = 0;

	auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, uniformHandle);
	PerEntityUniform[Id] = uniformHandle;

	m_Shader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);
}
