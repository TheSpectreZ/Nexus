#include "nxpch.h"
#include "SceneBuilder.h"
#include "Entity.h"
#include "Material.h"
#include "Renderer/Mesh.h"
#include "Assets/AssetManager.h"

Nexus::Ref<Nexus::SceneBuildData> Nexus::SceneBuildData::Create(Ref<Scene> scene, Ref<Shader> shader)
{
    auto ref = CreateRef<SceneBuildData>();
    ref->Build(scene, shader);
    return ref;
}

void Nexus::SceneBuildData::Build(Ref<Scene> scene, Ref<Shader> shader)
{
    this->shader = shader;

    // Per Scene
    {
        PerSceneHeap.hashId = CreateUUID();
        PerSceneHeap.set = 0;

        shader->AllocateShaderResourceHeap(PerSceneHeap);

        PerSceneUniform0.hashId = CreateUUID();
        PerSceneUniform0.set = 0;
        PerSceneUniform0.binding = 0;

        shader->AllocateUniformBuffer(PerSceneUniform0);
        shader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform0);

        PerSceneUniform1.hashId = CreateUUID();
        PerSceneUniform1.set = 0;
        PerSceneUniform1.binding = 1;
        
        shader->AllocateUniformBuffer(PerSceneUniform1);
        shader->BindUniformWithResourceHeap(PerSceneHeap, PerSceneUniform1);
    }

    // Per Material Heaps
    {
        Entity entity;
        auto view = scene->GetAllEntitiesWith<Component::Mesh>();
        for (auto& e : view)
        {
            entity = { e,scene.get() };

            auto& mesh = entity.GetComponent<Component::Mesh>();
            
            if (mesh.handle == NullUUID)
                continue;
        
            auto MeshAsset = AssetManager::Get<StaticMesh>(mesh.handle);
            auto& submeshes = MeshAsset->GetSubMeshes();
            for (auto& s : submeshes)
            {
                Ref<Material> material = AssetManager::Get<Material>(s.material);

                if (PerMaterialHeap.contains(material->GetID()))
                    continue;

                ResourceHeapHandle rh{};
                rh.hashId = CreateUUID();
                rh.set = 2;
                shader->AllocateShaderResourceHeap(rh);

                PerMaterialHeap[material->GetID()] = rh;

                UniformBufferHandle buf{};
                buf.hashId = CreateUUID();
                buf.set = 2;
                buf.binding = 0;
                shader->AllocateUniformBuffer(buf);

                PerMaterialUniform[material->GetID()] = buf;
                shader->BindUniformWithResourceHeap(rh, buf);

                CombinedImageSamplerHandle handle1{};
                handle1.texture = AssetManager::Get<Texture>(material->m_AlbedoMap.Image);
                handle1.sampler = AssetManager::Get<Sampler>(material->m_AlbedoMap.Sampler);
                handle1.set = 2;
                handle1.binding = 1;

                shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle1);

                CombinedImageSamplerHandle handle2{};
                handle2.texture = AssetManager::Get<Texture>(material->m_MetallicRoughnessMap.Image);
                handle2.sampler = AssetManager::Get<Sampler>(material->m_MetallicRoughnessMap.Sampler);
                handle2.set = 2;
                handle2.binding = 2;

                shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle2);

                CombinedImageSamplerHandle handle3{};
                handle3.texture = AssetManager::Get<Texture>(material->m_Normal.Image);
                handle3.sampler = AssetManager::Get<Sampler>(material->m_Normal.Sampler);
                handle3.set = 2;
                handle3.binding = 3;

                shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle3);
            }
        }
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
            PerEntityHeap[Identity.uuid] = heapHandle;

            UniformBufferHandle uniformHandle{};
            uniformHandle.hashId = CreateUUID();
            uniformHandle.set = 1;
            uniformHandle.binding = 0;
            
            shader->AllocateUniformBuffer(uniformHandle);
            PerEntityUniform[Identity.uuid] = uniformHandle;

            shader->BindUniformWithResourceHeap(heapHandle, uniformHandle);
        }
    }
    NEXUS_LOG_WARN("Scene Data Built");
}

void Nexus::SceneBuildData::Update(Ref<Scene> scene, Camera camera)
{
    // make this frequency based
    matrixBuffer[0] = camera.projection;
    matrixBuffer[1] = camera.view;
    shader->SetUniformData(PerSceneUniform0, &matrixBuffer);

    {
        m_SceneBuffer.camPos = camera.position;
        
        Entity entity;
     
        // PointLight
        auto pview = scene->GetAllEntitiesWith<Component::PointLight>();
        for (uint32_t i = 0; i < pview.size(); i++)
        {
            if (i >= PointLightLimit)
                break;

            entity = Entity(pview[i], scene.get());
            auto& tf = entity.GetComponent<Component::Transform>();
            auto& pl = entity.GetComponent<Component::PointLight>();

            m_SceneBuffer.pointLights[i].pos = tf.Translation;
            m_SceneBuffer.pointLights[i].col = pl.color;
            m_SceneBuffer.pointLights[i].size = pl.size;
            m_SceneBuffer.pointLights[i].intensity = pl.intensity;
            m_SceneBuffer.pointLights[i].faloff = pl.falloff;
        }

        for (uint32_t i = (uint32_t)pview.size(); i < PointLightLimit; i++)
        {
            m_SceneBuffer.pointLights[i].pos = glm::vec3(0.f);
            m_SceneBuffer.pointLights[i].col = glm::vec3(0.f);
        }

        if ((uint32_t)pview.size() < PointLightLimit)
            m_SceneBuffer.pLightCount = (float)pview.size();
        else
            m_SceneBuffer.pLightCount = PointLightLimit;

        //[Note] Think about Multiple Directional Lights.. 
        // DirectionalLight
        auto dview = scene->GetAllEntitiesWith<Component::DirectionalLight>();
        for (auto& e : dview)
        {
            entity = Entity(e, scene.get());
            auto& dl = entity.GetComponent<Component::DirectionalLight>();

            m_SceneBuffer.lightDir = glm::vec4(dl.direction, 0.0);
            m_SceneBuffer.lightCol = glm::vec4(dl.color, 0.0);
        }

        if (dview.empty())
        {
            m_SceneBuffer.lightDir = glm::vec4(0.0);
            m_SceneBuffer.lightCol = glm::vec4(0.0);
        }
        
        shader->SetUniformData(PerSceneUniform1, &m_SceneBuffer);
    }

    // Uniform
    {
        for (auto& u : PerMaterialUniform)
        {
            auto material = AssetManager::Get<Material>(u.first);

            materialBuffer[0] = material->m_AlbedoColor;
            materialBuffer[1] = { material->m_roughness,material->m_metalness,(float)material->m_AlbedoMap.TexCoord,(float)material->m_MetallicRoughnessMap.TexCoord };
            materialBuffer[2] = { material->useMR,material->useAlbedo,material->useNormal,0.f };

            shader->SetUniformData(u.second, &materialBuffer);
        }
    }
    
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
    shader->DeallocateUniformBuffer(PerSceneUniform0);
    shader->DeallocateUniformBuffer(PerSceneUniform1);

    for (auto& [k, v] : PerMaterialHeap)
    {
        shader->DeallocateShaderResourceHeap(v);
        shader->DeallocateUniformBuffer(PerMaterialUniform[k]);
    }

    for (auto& [k, v] : PerEntityHeap)
    {
        shader->DeallocateShaderResourceHeap(v);
        shader->DeallocateUniformBuffer(PerEntityUniform[k]);
    }
    PerEntityHeap.clear();
    PerEntityUniform.clear();
    PerMaterialHeap.clear();
    PerMaterialUniform.clear();

    NEXUS_LOG_WARN("Scene Data Destroyed");
}

void Nexus::SceneBuildData::OnMaterialCreation(UUID Id)
{
    Ref<Material> material = AssetManager::Get<Material>(Id);

    if (PerMaterialHeap.contains(material->GetID()))
        return;
    
    ResourceHeapHandle rh{};
    rh.hashId = CreateUUID();
    rh.set = 2;
    shader->AllocateShaderResourceHeap(rh);

    PerMaterialHeap[material->GetID()] = rh;

    UniformBufferHandle buf{};
    buf.hashId = CreateUUID();
    buf.set = 2;
    buf.binding = 0;
    shader->AllocateUniformBuffer(buf);

    PerMaterialUniform[material->GetID()] = buf;
    shader->BindUniformWithResourceHeap(rh, buf);

    CombinedImageSamplerHandle handle1{};
    handle1.texture = AssetManager::Get<Texture>(material->m_AlbedoMap.Image);
    handle1.sampler = AssetManager::Get<Sampler>(material->m_AlbedoMap.Sampler);
    handle1.set = 2;
    handle1.binding = 1;

    shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle1);

    CombinedImageSamplerHandle handle2{};
    handle2.texture = AssetManager::Get<Texture>(material->m_MetallicRoughnessMap.Image);
    handle2.sampler = AssetManager::Get<Sampler>(material->m_MetallicRoughnessMap.Sampler);
    handle2.set = 2;
    handle2.binding = 2;
    
    shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle2);
    
    CombinedImageSamplerHandle handle3{};
    handle3.texture = AssetManager::Get<Texture>(material->m_Normal.Image);
    handle3.sampler = AssetManager::Get<Sampler>(material->m_Normal.Sampler);
    handle3.set = 2;
    handle3.binding = 3;
    
    shader->BindTextureWithResourceHeap(PerMaterialHeap[material->GetID()], handle3);
}

void Nexus::SceneBuildData::OnSceneDestruction()
{
    shader->DeallocateShaderResourceHeap(PerSceneHeap);
    shader->DeallocateUniformBuffer(PerSceneUniform0);
    shader->DeallocateUniformBuffer(PerSceneUniform1);
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
