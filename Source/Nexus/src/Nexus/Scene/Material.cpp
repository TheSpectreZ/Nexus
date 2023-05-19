#include "nxpch.h"
#include "Material.h"

Nexus::Ref<Nexus::Material> Nexus::Material::Create(const MaterialCreateInfo& Info)
{
    Ref<Material> mat = CreateRef<Material>();
    
    mat->m_AlbedoMap = Info.albedo;
    mat->m_MetallicRoughnessMap = Info.metallicRoughness;
    mat->m_Normal = Info.normal;

    mat->useMR = Info.useMR;
    mat->useAlbedo = Info.useAlb;
    mat->useNormal = Info.useNormal;
    mat->m_AlbedoColor = Info.albedoColor;
    mat->m_roughness = Info.roughness;
    mat->m_metalness = Info.metalness;

    mat->m_Id = CreateUUID();

    return mat;
}

void Nexus::Material::SetUseAlbedo(bool use)
{
    useAlbedo = use ? 1.f : 0.f;
}


void Nexus::Material::SetUseNormal(bool use)
{
    useNormal = use ? 1.f : 0.f;
}

void Nexus::Material::SetUseMetallicRoughness(bool use)
{
    useMR = use ? 1.f : 0.f;
}

void Nexus::Material::SetRoughness(float roughness)
{
    m_roughness = roughness;
}

void Nexus::Material::SetMetalness(float metalness)
{
    m_metalness = metalness;
}

void Nexus::Material::SetAlbedoColor(const glm::vec4 color)
{
    m_AlbedoColor = color;
}