#include "NxRenderEngine/RenderableScene.h"
#include "NxRenderEngine/ResourcePool.h"

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

	Entity entity;
	auto view = m_Scene->GetAllEntitiesWith<Component::DirectionalLight>();
	for (auto& e : view)
	{
		entity = { e,m_Scene.get() };
	
		auto& DL = entity.GetComponent<Component::DirectionalLight>();
		m_SceneBuffer.lightDir = DL.direction;
		m_SceneBuffer.lightCol = DL.color;
	}
	
	auto sceneBuf = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform1.hashId);
	sceneBuf->Update((void*)&m_SceneBuffer);
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

		for (auto& sb : RTMesh->GetSubmeshes())
		{
			if (sb.materialIndex != UINT64_MAX)
			{
				if (!PerMaterialHeap.contains(sb.materialIndex))
					CreateMaterialResource(sb.materialIndex);

				queue->BindShaderResourceHeap(m_Shader, PerMaterialHeap[sb.materialIndex]);
				queue->DrawIndices(sb.indexSize, 1, sb.indexOffset, 0, 0);
			}
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
		specs.sampler.Far = SamplerFilter::Linear;
		specs.sampler.Near = SamplerFilter::Linear;
		specs.sampler.U = SamplerWrapMode::Repeat;
		specs.sampler.V = SamplerWrapMode::Repeat;
		specs.sampler.W = SamplerWrapMode::Repeat;

		m_Sampler = ResourcePool::Get()->GetSampler(specs);
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

void Nexus::RenderableScene::CreateMaterialResource(UUID Id)
{
	ResourceHeapHandle heapHandle{};
	heapHandle.hashId = UUID();
	heapHandle.set = 2;

	m_Shader->AllocateShaderResourceHeap(heapHandle);
	PerMaterialHeap[Id] = heapHandle;

	UniformBufferHandle uniformHandle{};
	uniformHandle.hashId = UUID();
	uniformHandle.set = 2;
	uniformHandle.binding = 0;

	auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, uniformHandle);
	PerMaterialUniform[Id] = uniformHandle;
	m_Shader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);
	
	Ref<RenderableMaterial> material = ResourcePool::Get()->GetRenderableMaterial(Id);
	auto factors = material->GetParams()._factors;
	buff->Update(&factors);

	Ref<Sampler> sampler = ResourcePool::Get()->GetSampler(11122);
	
	CombinedImageSamplerHandle imageHandle;
	imageHandle.set = 2;
	
	UUID def = UUID((uint64_t)0);
	Ref<Texture> defaultTex = ResourcePool::Get()->GetTexture(def);

	if (material->GetParams()._factors.useBaseColorMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::Albedo]);
		imageHandle.texture = material->GetParams()._Maps[TextureType::Albedo];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 1;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetParams()._factors.pbrType == 1.f)
	{
		if (material->GetParams()._factors.useSurfaceMap > -1)
		{
			imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::MetallicRoughness]);
			imageHandle.texture = material->GetParams()._Maps[TextureType::MetallicRoughness];
		}
	}
	else if (material->GetParams()._factors.pbrType == 2.f)
	{
		if (material->GetParams()._factors.useSurfaceMap > -1)
		{
			imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::SpecularGlossiness]);
			imageHandle.texture = material->GetParams()._Maps[TextureType::SpecularGlossiness];
		}
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 2;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetParams()._factors.useNormalMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::Normal]);
		imageHandle.texture = material->GetParams()._Maps[TextureType::Normal];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 3;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	if (material->GetParams()._factors.useOculsionMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::Occulsion]);
		imageHandle.texture = material->GetParams()._Maps[TextureType::Occulsion];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}
	
	imageHandle.binding = 4;
	m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	//if (material->GetParams()._factors.useEmissiveMap > -1)
	//{
	//	imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureType::Emissive]);
	//	imageHandle.texture = material->GetParams()._Maps[TextureType::Occulsion];
	//}
	//else
	//{
	//	imageHandle.sampler = sampler;
	//	imageHandle.texture = defaultTex;
	//}
	//
	//imageHandle.binding = 5;
	//m_Shader->BindTextureWithResourceHeap(heapHandle, imageHandle);
}

