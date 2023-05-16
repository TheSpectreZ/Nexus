#include "nxpch.h"
#include "Material.h"

Nexus::Ref<Nexus::Material> Nexus::Material::Create(const MaterialCreateInfo& Info)
{
    Ref<Material> mat = CreateRef<Material>();
    
    mat->m_AlbedoMap = Info.albedo;
    mat->m_MetallicRoughnessMap = Info.metallicRoughness;

    mat->useMR = Info.useMR;
    mat->useAlbedo = Info.useAlb;
    mat->m_AlbedoColor = Info.albedoColor;
    mat->m_roughness = Info.roughness;
    mat->m_metalness = Info.metalness;

    return mat;
}
