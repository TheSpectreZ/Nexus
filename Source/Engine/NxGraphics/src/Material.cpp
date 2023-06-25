#include "NxGraphics/Material.h"

Nexus::PBRMaterial::PBRMaterial(const PBRMaterialSpecifications& specs)
{
	m_Params = specs.params;
	m_AlbedoMap = specs.albedo;
	m_NormalMap = specs.normal;
	m_MetallicRoughnessMap = specs.metallicRoughness;
}
