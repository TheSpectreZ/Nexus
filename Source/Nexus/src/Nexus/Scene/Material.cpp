#include "nxpch.h"
#include "Material.h"

Nexus::Ref<Nexus::Material> Nexus::Material::Create(const MaterialCreateInfo& Info)
{
    Ref<Material> mat = CreateRef<Material>();
    
    mat->m_AlbedoMap = Info.albedo;
    mat->m_AlbedoColor = Info.albedoColor;

    return mat;
}
