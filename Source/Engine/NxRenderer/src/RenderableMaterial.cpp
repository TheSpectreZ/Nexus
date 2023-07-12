#include "NxRenderer/RenderableMaterial.h"
#include "NxRenderer/ResourcePool.h"

Nexus::RenderableMaterial::RenderableMaterial(const Meshing::Material& params)
{
	m_Params._factors.albedo = params.specularGlossiness.support ? params.specularGlossiness.aldeboColor : params.metalicRoughness.albedoColor;
	m_Params._factors.glossiness = params.specularGlossiness.glossiness;
	m_Params._factors.metalness = params.metalicRoughness.metallic;
	m_Params._factors.roughness = params.metalicRoughness.roughness;
	m_Params._factors.specular = params.specularGlossiness.specular;
	m_Params._factors.emissive = params.emissiveColor;
}
