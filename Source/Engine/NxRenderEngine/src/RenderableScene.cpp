#include "NxRenderEngine/RenderableScene.h"
#include "NxRenderEngine/ResourcePool.h"

Nexus::RenderableScene::RenderableScene(Ref<Shader> pbr,Ref<Shader> skybox)
	:m_pbrShader(pbr),m_skyBoxShader(skybox)
{
	Initialize();
}

Nexus::RenderableScene::~RenderableScene()
{
	Destroy();
}

void Nexus::RenderableScene::Prepare(Ref<Scene> scene)
{
	auto cameraBuf = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform0.hashId);
	cameraBuf->Update((void*)scene->GetCamera());

	auto& rootEntity = scene->GetRootEntity();

	auto& DL = rootEntity.directionalLight;
	m_SceneBuffer.lightDir = DL.direction;
	m_SceneBuffer.lightCol = DL.color;
	
	auto& Env = rootEntity.environment;
	m_SceneBuffer.exposure = Env.exposure;

	if (Env.handle && m_SceneBuffer.useIBL < 0)
	{
		m_SceneBuffer.useIBL = 1;
		// Cause this works as getter too... [To-Do : Make this more sensible]
		auto env = ResourcePool::Get()->AllocateEnvironment(Env.handle);

		ImageHandle handle{};
		handle.set = 0;
		handle.binding = 3;
		handle.texture = env->IrradianceMap;
		handle.sampler = ResourcePool::Get()->GetSampler(11122);
		handle.Type = ShaderResourceType::SampledImage;
		
		m_pbrShader->BindTextureWithResourceHeap(PerSceneHeap, handle);
		
		handle.binding = 4;
		handle.texture = env->specularMap;
		
		m_pbrShader->BindTextureWithResourceHeap(PerSceneHeap, handle);

		handle.binding = 1;
		handle.texture = env->envMap;

		m_skyBoxShader->BindTextureWithResourceHeap(SkyBoxHeap, handle);
	}

	auto sceneBuf = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform1.hashId);
	sceneBuf->Update((void*)&m_SceneBuffer);
}

void Nexus::RenderableScene::DrawSkybox(Ref<CommandQueue> queue)
{
	queue->BindShaderResourceHeap(m_skyBoxShader, SkyBoxHeap, PipelineBindPoint::Graphics);
	
	auto RTMesh = ResourcePool::Get()->GetAsset<RenderableMesh>(DEFAULT_MESH_RESOURCE);

	auto Ib = RTMesh->GetIndexBuffer();

	queue->BindVertexBuffer(RTMesh->GetVertexBuffer());
	queue->BindIndexBuffer(Ib);
	queue->DrawIndices(Ib->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
}

void Nexus::RenderableScene::DrawScene(Ref<CommandQueue> queue, Ref<Scene> scene)
{
	queue->BindShaderResourceHeap(m_pbrShader, PerSceneHeap, PipelineBindPoint::Graphics);

	Entity entity;
	auto view = scene->GetAllEntitiesWith<Component::Mesh>();
	for (auto& e : view)
	{
		entity = { e,scene.get() };

		auto& MeshComponent = entity.GetComponent<Component::Mesh>();
		if (!MeshComponent.handle)
			continue;
		
		auto& Identity = entity.GetComponent<Component::Identity>();
		if (!PerEntityHeap.contains(Identity.uuid))
			CreateEntityResource(Identity.uuid);

		auto Transform = entity.GetComponent<Component::Transform>().GetTransform();
		auto buff = ResourcePool::Get()->GetUniformBuffer(PerEntityUniform[Identity.uuid].hashId);
		buff->Update(glm::value_ptr(Transform));
		
		auto RTMesh = ResourcePool::Get()->GetAsset<RenderableMesh>(MeshComponent.handle);

		queue->BindShaderResourceHeap(m_pbrShader, PerEntityHeap[Identity.uuid],PipelineBindPoint::Graphics);
		queue->BindVertexBuffer(RTMesh->GetVertexBuffer());
		queue->BindIndexBuffer(RTMesh->GetIndexBuffer());
	
		if (MeshComponent.materialTable.empty())
		{
			queue->BindShaderResourceHeap(m_pbrShader, DefaultMaterialHeap, PipelineBindPoint::Graphics);
			queue->DrawIndices(RTMesh->GetIndexBuffer()->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
		}
		else
		{
			for (auto& sb : RTMesh->GetSubmeshes())
			{
				auto matIndex = MeshComponent.materialTable.at(sb.materialIndex);
				if (!PerMaterialHeap.contains(matIndex))
					CreateMaterialResource(matIndex);

				queue->BindShaderResourceHeap(m_pbrShader, PerMaterialHeap[matIndex], PipelineBindPoint::Graphics);
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
		m_pbrShader->AllocateShaderResourceHeap(PerSceneHeap);

		// Camera
		PerSceneUniform0.hashId = UUID();
		PerSceneUniform0.set = 0;
		PerSceneUniform0.binding = 0;
		auto cbuff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, PerSceneUniform0);

		// Scene
		PerSceneUniform1.hashId = UUID();
		PerSceneUniform1.set = 0;
		PerSceneUniform1.binding = 1;
		auto sbuff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, PerSceneUniform1);

		m_pbrShader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform0.binding, cbuff);
		m_pbrShader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform1.binding, sbuff);

		// Environment
		ImageHandle handle{};
		handle.set = 0;
		handle.binding = 2;
		handle.texture = EnvironmentBuilder::GetBRDFLut();
		handle.sampler = ResourcePool::Get()->GetSampler(11122);
		handle.Type = ShaderResourceType::SampledImage;

		m_pbrShader->BindTextureWithResourceHeap(PerSceneHeap, handle);

		// Default Material
		DefaultMaterialHeap.hashId = UUID();
		DefaultMaterialHeap.set = 2;

		m_pbrShader->AllocateShaderResourceHeap(DefaultMaterialHeap);
		
		DefaultMaterialUniform.hashId = UUID();
		DefaultMaterialUniform.set = 2;
		DefaultMaterialUniform.binding = 0;

		auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, DefaultMaterialUniform);
		m_pbrShader->BindUniformWithResourceHeap(DefaultMaterialHeap, DefaultMaterialUniform.binding, buff);

		Ref<RenderableMaterial> material = ResourcePool::Get()->GetAsset<RenderableMaterial>(DEFAULT_MATERIAL_RESOURCE);
		auto factors = material->GetParams()._factors;
		buff->Update(&factors);
	}

	// Skybox
	{
		SkyBoxHeap.hashId = UUID();
		SkyBoxHeap.set = 0;
		m_skyBoxShader->AllocateShaderResourceHeap(SkyBoxHeap);

		auto buffer = ResourcePool::Get()->GetUniformBuffer(PerSceneUniform0.hashId);
		m_skyBoxShader->BindUniformWithResourceHeap(SkyBoxHeap, 0, buffer);
	}
}

void Nexus::RenderableScene::Destroy()
{
	m_pbrShader->DeallocateShaderResourceHeap(PerSceneHeap);
	ResourcePool::Get()->DeallocateUniformBuffer(PerSceneUniform0.hashId);
	ResourcePool::Get()->DeallocateUniformBuffer(PerSceneUniform1.hashId);

	for (auto& [k, v] : PerEntityHeap)
	{
		m_pbrShader->DeallocateShaderResourceHeap(v);
		ResourcePool::Get()->DeallocateUniformBuffer(PerEntityUniform[k].hashId);
	}
	PerEntityHeap.clear();
	PerEntityUniform.clear();
	
	for (auto& [k, v] : PerMaterialHeap)
	{
		m_pbrShader->DeallocateShaderResourceHeap(v);
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

	m_pbrShader->AllocateShaderResourceHeap(heapHandle);
	PerEntityHeap[Id] = heapHandle;

	UniformBufferHandle uniformHandle{};
	uniformHandle.hashId = UUID();
	uniformHandle.set = 1;
	uniformHandle.binding = 0;
	
	auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, uniformHandle);
	PerEntityUniform[Id] = uniformHandle;

	m_pbrShader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);
}

void Nexus::RenderableScene::CreateMaterialResource(UUID Id)
{
	ResourceHeapHandle heapHandle{};
	heapHandle.hashId = UUID();
	heapHandle.set = 2;

	m_pbrShader->AllocateShaderResourceHeap(heapHandle);
	PerMaterialHeap[Id] = heapHandle;

	UniformBufferHandle uniformHandle{};
	uniformHandle.hashId = UUID();
	uniformHandle.set = 2;
	uniformHandle.binding = 0;

	auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, uniformHandle);
	PerMaterialUniform[Id] = uniformHandle;
	m_pbrShader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);
	
	Ref<RenderableMaterial> material = ResourcePool::Get()->GetAsset<RenderableMaterial>(Id);
	auto factors = material->GetParams()._factors;
	buff->Update(&factors);

	Ref<Sampler> sampler = ResourcePool::Get()->GetSampler(11122);
	
	ImageHandle	imageHandle;
	imageHandle.Type = ShaderResourceType::SampledImage;
	imageHandle.set = 2;
	
	Ref<Texture> defaultTex = ResourcePool::Get()->GetAsset<Texture>(DEFAULT_TEXTURE_RESOURCE);

	if (material->GetParams()._factors.useBaseColorMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::Albedo]);
		imageHandle.texture = material->GetParams()._Maps[TextureMapType::Albedo];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 1;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetParams()._factors.pbrType == 1.f)
	{
		if (material->GetParams()._factors.useSurfaceMap > -1)
		{
			imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::MetallicRoughness]);
			imageHandle.texture = material->GetParams()._Maps[TextureMapType::MetallicRoughness];
		}
	}
	else if (material->GetParams()._factors.pbrType == 2.f)
	{
		if (material->GetParams()._factors.useSurfaceMap > -1)
		{
			imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::SpecularGlossiness]);
			imageHandle.texture = material->GetParams()._Maps[TextureMapType::SpecularGlossiness];
		}
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 2;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetParams()._factors.useNormalMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::Normal]);
		imageHandle.texture = material->GetParams()._Maps[TextureMapType::Normal];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 3;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	if (material->GetParams()._factors.useOculsionMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::Occulsion]);
		imageHandle.texture = material->GetParams()._Maps[TextureMapType::Occulsion];
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}
	
	imageHandle.binding = 4;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	//if (material->GetParams()._factors.useEmissiveMap > -1)
	//{
	//	imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetParams()._Samplers[TextureMapType::Emissive]);
	//	imageHandle.texture = material->GetParams()._Maps[TextureMapType::Occulsion];
	//}
	//else
	//{
	//	imageHandle.sampler = sampler;
	//	imageHandle.texture = defaultTex;
	//}
	//
	//imageHandle.binding = 5;
	//m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);
}

