#include "nxpch.h"
#include "Material.h"

Nexus::Ref<Nexus::Material> Nexus::Material::Create()
{
    return CreateRef<Material>();
}
