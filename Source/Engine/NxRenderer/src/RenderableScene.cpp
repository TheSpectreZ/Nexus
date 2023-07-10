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
	cameraBuf->Update((void*)m_Scene->GetCamera());

	auto sceneBuf = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform1.hashId);
	sceneBuf->Update((void*)&m_SceneBuffer);

	Entity entity;
	auto view = m_Scene->GetAllEntitiesWith<Component::DirectionalLight>();
	for (auto& e : view)
	{
		entity = { e,m_Scene.get() };

		auto& DL = entity.GetComponent<Component::DirectionalLight>();
		m_SceneBuffer.lightDir = DL.direction;
		m_SceneBuffer.lightCol = DL.color;
	}
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

		if (!RTMesh->GetMaterialTable())
			return;

		auto& Materials = RTMesh->GetMaterialTable()->GetMaterials();

		for (auto& sm : RTMesh->GetSubmeshes())
		{
			auto MatId = Materials[sm.materialIndex].m_Id;
			if (!PerMaterialHeap.contains(MatId))
				CreateMaterialResource(Materials[sm.materialIndex]);

			queue->BindShaderResourceHeap(m_Shader, PerMaterialHeap[MatId]);
			queue->DrawIndices(sm.IndexSize, 1, sm.IndexOff, 0, 0);
		}
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
		auto cbuff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, PerSceneUniform0);

		// Scene
		PerSceneUniform1.hashId = UUID();
		PerSceneUniform1.set = 0;
		PerSceneUniform1.binding = 1;
		auto sbuff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, PerSceneUniform1);

		m_Shader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform0.binding, cbuff);
		m_Shader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform1.binding, sbuff);
	}

	// Sampler
	{
		SamplerSpecification specs{};
		specs.Far = SamplerFilter::Linear;
		specs.Near = SamplerFilter::Linear;
		specs.U = SamplerWrapMode::Repeat;
		specs.V = SamplerWrapMode::Repeat;
		specs.W = SamplerWrapMode::Repeat;

		m_Sampler = GraphicsInterface::CreateSampler(specs);
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
	
	for (auto& [k, v] : PerMaterialHeap)
	{
		m_Shader->DeallocateShaderResourceHeap(v);
		ResourcePool::Get()->DeallocateUniformBuffer(PerMaterialUniform[k].hashId);
	}
	PerMaterialHeap.clear();
	PerMaterialUniform.clear();
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

void Nexus::RenderableScene::CreateMaterialResource(const RenderableMaterial& material)
{
	ResourceHeapHandle heapHandle{};
	heapHandle.hashId = UUID();
	heapHandle.set = 2;

	m_Shader->AllocateShaderResourceHeap(heapHandle);
	PerMaterialHeap[material.m_Id] = heapHandle;

	UniformBufferHandle uniformHandle{};
	uniformHandle.hashId = UUID();
	uniformHandle.set = 2;
	uniformHandle.binding = 0;

	auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, uniformHandle);
	buff->Update((void*)&material.m_Params);
	PerMaterialUniform[material.m_Id] = uniformHandle;

	m_Shader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);

	CombinedImageSamplerHandle imageHandle{};
	imageHandle.set = 2;
	imageHandle.sampler = m_Sampler;

	imageHandle.binding = 1;
	imageHandle.texture = material.m_AlbedoMap;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	imageHandle.binding = 2;
	imageHandle.texture = material.m_MetalicRoughnessMap;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	imageHandle.binding = 3;
	imageHandle.texture = material.m_NormalMap;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);
}
