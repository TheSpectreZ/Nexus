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
	m_Params._factors.useNormal = 0.f;

	if (params.specularGlossiness.support)
	{
		if (params.specularGlossiness.albedoTexture != UINT64_MAX)
			m_Params._Maps[TextureType::Albedo] = ResourcePool::Get()->GetTexture(params.specularGlossiness.albedoTexture);
	}
	else
	{
		if (params.metalicRoughness.albedoTexture != UINT64_MAX)
			m_Params._Maps[TextureType::Albedo] = ResourcePool::Get()->GetTexture(params.metalicRoughness.albedoTexture);
	}

	if (params.normalTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureType::Normal] = ResourcePool::Get()->GetTexture(params.normalTexture);
		m_Params._factors.useNormal = 1.f;
	}

	if (params.occulsionTexture != UINT64_MAX)
		m_Params._Maps[TextureType::Occulsion] = ResourcePool::Get()->GetTexture(params.occulsionTexture);
	if (params.emissiveTexture != UINT64_MAX)
		m_Params._Maps[TextureType::Emissive] = ResourcePool::Get()->GetTexture(params.emissiveTexture);
	if (params.metalicRoughness.metallicRoughnessTexture != UINT64_MAX)
		m_Params._Maps[TextureType::MetallicRoughness] = ResourcePool::Get()->GetTexture(params.metalicRoughness.metallicRoughnessTexture);
	if (params.normalTexture != UINT64_MAX)
		m_Params._Maps[TextureType::SpecularGlossiness] = ResourcePool::Get()->GetTexture(params.specularGlossiness.specularGlossinessTexture);
}
