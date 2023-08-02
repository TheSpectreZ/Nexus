#include "NxRenderEngine/RenderableScene.h"
#include "NxRenderEngine/ResourcePool.h"
#include "NxRenderEngine/RenderableAsset.h"

#include "NxCore/Registry.h"

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
		auto env = ResourcePool::Get()->GetEnvironment(Env.handle);

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
	
	UUID Id = AssetRegistry::Get()->LookUp(AssetFilePath("Resources/Meshes/Mesh/Cube.NxMeshSource"));
	auto RTMesh = RenderableMesh::GetFromPool(Id);

	auto Ib = RTMesh->GetIndexBuffer(0);

	queue->BindVertexBuffer(RTMesh->GetVertexBuffer(0));
	queue->BindIndexBuffer(Ib);
	queue->DrawIndices(Ib->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
}

#define GET_LOCAL_TRANSFORM(t,r,s,m) glm::translate(glm::mat4(1.f), t) * glm::mat4(r) * glm::scale(glm::mat4(1.f),s) * m

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
		
		auto RTMesh = RenderableMesh::GetFromPool(MeshComponent.handle);

		auto& Identity = entity.GetComponent<Component::Identity>();
		if (!PerEntityHeap.contains(Identity.uuid))
			CreateEntityResource(Identity.uuid,RTMesh->GetCount());

		auto Transform = entity.GetComponent<Component::Transform>().GetTransform();

		if (MeshComponent.skeleton)
		{
			auto skel = RTMesh->GetSkeleton();

			for (auto& node : skel.nodes)
			{
				if (node.parent < 0 || node.mesh == UINT32_MAX)
					continue;

				glm::mat4 m = GET_LOCAL_TRANSFORM(node.translation, node.rotation, node.scale, node.matrix);
				
				Meshing::Node* p = &skel.nodes[node.parent];
				while (p)
				{
					m = GET_LOCAL_TRANSFORM(p->translation, p->rotation, p->scale, p->matrix) * m;
					p = p->parent > 0 ? &skel.nodes[p->parent] : nullptr;
				}

				m = Transform * m;

				auto buff = ResourcePool::Get()->GetUniformBuffer(PerEntityUniform[Identity.uuid.operator size_t() + (uint64_t)node.mesh].hashId);
				buff->Update(glm::value_ptr(m));

				queue->BindShaderResourceHeap(m_pbrShader, PerEntityHeap[Identity.uuid.operator size_t() + (uint64_t)node.mesh], PipelineBindPoint::Graphics);
				queue->BindVertexBuffer(RTMesh->GetVertexBuffer(node.mesh));
				queue->BindIndexBuffer(RTMesh->GetIndexBuffer(node.mesh));

				if (MeshComponent.materialTable.empty())
				{
					queue->BindShaderResourceHeap(m_pbrShader, DefaultMaterialHeap, PipelineBindPoint::Graphics);
					queue->DrawIndices(RTMesh->GetIndexBuffer(node.mesh)->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
				}
				else
				{
					for (auto& sb : RTMesh->GetSubmeshes(node.mesh))
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
		else
		{
			for (auto i = 0; i < RTMesh->GetCount(); i++)
			{
				auto buff = ResourcePool::Get()->GetUniformBuffer(PerEntityUniform[Identity.uuid.operator size_t() + i].hashId);
				buff->Update(glm::value_ptr(Transform));

				queue->BindShaderResourceHeap(m_pbrShader, PerEntityHeap[Identity.uuid.operator size_t() + i], PipelineBindPoint::Graphics);
				queue->BindVertexBuffer(RTMesh->GetVertexBuffer(i));
				queue->BindIndexBuffer(RTMesh->GetIndexBuffer(i));

				if (MeshComponent.materialTable.empty())
				{
					queue->BindShaderResourceHeap(m_pbrShader, DefaultMaterialHeap, PipelineBindPoint::Graphics);
					queue->DrawIndices(RTMesh->GetIndexBuffer(i)->GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
				}
				else
				{
					for (auto& sb : RTMesh->GetSubmeshes(i))
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

		UUID dId = AssetRegistry::Get()->LookUp(AssetFilePath("Resources/Material/Default.NxMat"));
		Ref<RenderableMaterial> material = RenderableMaterial::GetFromPool(dId);
		auto factors = material->GetBuffer();
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
	m_skyBoxShader->DeallocateShaderResourceHeap(SkyBoxHeap);

	m_pbrShader->DeallocateShaderResourceHeap(DefaultMaterialHeap);
	ResourcePool::Get()->DeallocateUniformBuffer(DefaultMaterialUniform.hashId);

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

void Nexus::RenderableScene::CreateEntityResource(UUID Id, uint64_t meshCount)
{
	ResourceHeapHandle heapHandle{};
	heapHandle.set = 1;

	UniformBufferHandle uniformHandle{};
	uniformHandle.set = 1;
	uniformHandle.binding = 0;
	
	for (auto i = 0; i < meshCount; i++)
	{
		heapHandle.hashId = UUID();
		m_pbrShader->AllocateShaderResourceHeap(heapHandle);
		PerEntityHeap[Id.operator size_t() + i] = heapHandle;

		uniformHandle.hashId = UUID();
		auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_pbrShader, uniformHandle);
		PerEntityUniform[Id.operator size_t() + i] = uniformHandle;

		m_pbrShader->BindUniformWithResourceHeap(heapHandle, uniformHandle.binding, buff);
	}
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
	
	Ref<RenderableMaterial> material = RenderableMaterial::GetFromPool(Id);
	auto factors = material->GetBuffer();
	buff->Update(&factors);

	
	ImageHandle	imageHandle;
	imageHandle.Type = ShaderResourceType::SampledImage;
	imageHandle.set = 2;
	
	UUID TId = AssetRegistry::Get()->LookUp(AssetFilePath("Resources/Textures/white.NxTex"));
	auto RT = RenderableTexture::GetFromPool(TId);
	Ref<Texture> defaultTex = RT->GetTexture();
	Ref<Sampler> sampler = ResourcePool::Get()->GetSampler(RT->GetSamplerHash());

	if (material->GetBuffer().useBaseColorMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetMaps().base->GetSamplerHash());
		imageHandle.texture = material->GetMaps().base->GetTexture();
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 1;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetBuffer().useSurfaceMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetMaps().surface->GetSamplerHash());
		imageHandle.texture = material->GetMaps().surface->GetTexture();
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 2;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);

	if (material->GetBuffer().useNormalMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetMaps().normal->GetSamplerHash());
		imageHandle.texture = material->GetMaps().normal->GetTexture();
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}

	imageHandle.binding = 3;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	if (material->GetBuffer().useOcculsionMap > -1)
	{
		imageHandle.sampler = ResourcePool::Get()->GetSampler(material->GetMaps().occulsion->GetSamplerHash());
		imageHandle.texture = material->GetMaps().occulsion->GetTexture();
	}
	else
	{
		imageHandle.sampler = sampler;
		imageHandle.texture = defaultTex;
	}
	
	imageHandle.binding = 4;
	m_pbrShader->BindTextureWithResourceHeap(heapHandle, imageHandle);
	
	//if (material->GetBuffer.useEmissiveMap > -1)
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

