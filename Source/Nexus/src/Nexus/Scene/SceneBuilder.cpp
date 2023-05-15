#include "nxpch.h"
#include "SceneBuilder.h"
#include "Entity.h"
#include "Material.h"
#include "Assets/AssetManager.h"

Nexus::Ref<Nexus::SceneBuildData> Nexus::SceneBuildData::Build(Ref<Scene> scene, Ref<Shader> shader)
{
    Ref<SceneBuildData> data = CreateRef<SceneBuildData>();
    data->shader = shader;

    data->sampler = Sampler::Create(SamplerFilter::Nearest, SamplerFilter::Linear,SamplerWrapMode::Repeat,SamplerWrapMode::Repeat,SamplerWrapMode::Repeat);

    // Per Scene
    {
        data->PerSceneHeap.hashId = CreateUUID();
        data->PerSceneHeap.set = 0;

        shader->AllocateShaderResourceHeap(data->PerSceneHeap);

        data->PerSceneUniform.hashId = CreateUUID();
        data->PerSceneUniform.set = 0;
        data->PerSceneUniform.binding = 0;

        shader->AllocateUniformBuffer(data->PerSceneUniform);
        shader->BindUniformWithResourceHeap(data->PerSceneHeap, data->PerSceneUniform);
    }

    // Per Entity Heaps
    {
        Entity entity;
        auto view = scene->GetAllEntitiesWith<Component::Identity>();
        for (auto& e : view)
        {
            entity = Entity(e, scene.get());
            auto& Identity = entity.GetComponent<Component::Identity>();

            ResourceHeapHandle heapHandle{};
            heapHandle.hashId = CreateUUID();
            heapHandle.set = 1;

            shader->AllocateShaderResourceHeap(heapHandle);
            data->PerEntityHeap[Identity.uuid] = heapHandle;

            UniformBufferHandle uniformHandle{};
            uniformHandle.hashId = CreateUUID();
            uniformHandle.set = 1;
            uniformHandle.binding = 0;
            
            shader->AllocateUniformBuffer(uniformHandle);
            data->PerEntityUniform[Identity.uuid] = uniformHandle;

            shader->BindUniformWithResourceHeap(heapHandle, uniformHandle);
        }
    }
    NEXUS_LOG_WARN("Scene Data Built");

    return data;
}

void Nexus::SceneBuildData::Update(Ref<Scene> scene, Camera camera)
{
    // make this frequency based
    matrixBuffer[0] = camera.projection;
    matrixBuffer[1] = camera.view;

    shader->SetUniformData(PerSceneUniform, &matrixBuffer);

    // Per Entity Transforms
    {
        Entity entity;
        auto view = scene->GetAllEntitiesWith<Component::Identity>();
        for (auto& e : view)
        {
            entity = Entity(e, scene.get());
            auto& Identity = entity.GetComponent<Component::Identity>();
            auto& Handle = PerEntityUniform[Identity.uuid];

            auto Transform = entity.GetComponent<Component::Transform>().GetTransform();

            shader->SetUniformData(Handle, glm::value_ptr(Transform));
        }
    }
}

void Nexus::SceneBuildData::Destroy()
{
    shader->DeallocateShaderResourceHeap(PerSceneHeap);
    shader->DeallocateUniformBuffer(PerSceneUniform);

    for (auto& [k, v] : PerMaterialHeap)
    {
        shader->DeallocateShaderResourceHeap(v);
    }

    for (auto& [k, v] : PerEntityHeap)
    {
        shader->DeallocateShaderResourceHeap(v);
        shader->DeallocateUniformBuffer(PerEntityUniform[k]);
    }
    PerEntityHeap.clear();
    PerEntityUniform.clear();
    PerMaterialHeap.clear();
    PerMaterialSamplerImage.clear();

    NEXUS_LOG_WARN("Scene Data Destroyed");
}

void Nexus::SceneBuildData::OnMaterialCreation(UUID Id)
{
    Ref<Material> material = AssetManager::Get<Material>(Id);

    ResourceHeapHandle rh{};
    rh.hashId = CreateUUID();
    rh.set = 2;
    shader->AllocateShaderResourceHeap(rh);

    PerMaterialHeap[material->GetID()] = rh;

    CombinedImageSamplerHandle handle{};
    handle.texture = AssetManager::Get<Texture>(material->GetAlbedo());
    handle.sampler = sampler;
    handle.set = 2;
    handle.binding = 0;

    shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle);
    PerMaterialSamplerImage[material->GetID()] = handle;
}

void Nexus::SceneBuildData::OnSceneDestruction()
{
    shader->DeallocateShaderResourceHeap(PerSceneHeap);
    shader->DeallocateUniformBuffer(PerSceneUniform);
}

void Nexus::SceneBuildData::OnEntityCreation(Entity e)
{
    auto& Identity = e.GetComponent<Component::Identity>();

    ResourceHeapHandle heapHandle{};
    heapHandle.hashId = CreateUUID();
    heapHandle.set = 1;

    shader->AllocateShaderResourceHeap(heapHandle);
    PerEntityHeap[Identity.uuid] = heapHandle;

    UniformBufferHandle uniformHandle{};
    uniformHandle.hashId = CreateUUID();
    uniformHandle.set = 1;
    uniformHandle.binding = 0;

    shader->AllocateUniformBuffer(uniformHandle);
    PerEntityUniform[Identity.uuid] = uniformHandle;

    shader->BindUniformWithResourceHeap(heapHandle, uniformHandle);
}

void Nexus::SceneBuildData::OnEntityDestruction(Entity e)
{
    UUID id = e.GetComponent<Component::Identity>().uuid;

    shader->DeallocateShaderResourceHeap(PerEntityHeap[id]);
    shader->DeallocateUniformBuffer(PerEntityUniform[id]);

    PerEntityHeap.erase(id);
    PerEntityUniform.erase(id);
}
