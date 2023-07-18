#include "NxRenderEngine/RenderableMaterial.h"
#include "NxRenderEngine/ResourcePool.h"

Nexus::RenderableMaterial::RenderableMaterial(const Meshing::Material& params, const std::unordered_map<TextureMapType, uint32_t>& Samplers)
{
	m_Params._Samplers = Samplers;

	m_Params._factors.albedo = params.specularGlossiness.support ? params.specularGlossiness.aldeboColor : params.metalicRoughness.albedoColor;
	m_Params._factors.glossiness = params.specularGlossiness.glossiness;
	m_Params._factors.metalness = params.metalicRoughness.metallic;
	m_Params._factors.roughness = params.metalicRoughness.roughness;
	m_Params._factors.specular = params.specularGlossiness.specular;
	m_Params._factors.emissive = params.emissiveColor;
	
	if (params.specularGlossiness.support)
	{
		m_Params._factors.pbrType = 2;
		if (params.specularGlossiness.albedoTexture != UINT64_MAX)
		{
			m_Params._Maps[TextureMapType::Albedo] = ResourcePool::Get()->GetTexture(params.specularGlossiness.albedoTexture);
			m_Params._factors.useBaseColorMap = 1;
		}
	}
	else
	{
		m_Params._factors.pbrType = 1;
		if (params.metalicRoughness.albedoTexture != UINT64_MAX)
		{
			m_Params._Maps[TextureMapType::Albedo] = ResourcePool::Get()->GetTexture(params.metalicRoughness.albedoTexture);
			m_Params._factors.useBaseColorMap = 1;
		}
	}

	if (params.normalTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureMapType::Normal] = ResourcePool::Get()->GetTexture(params.normalTexture);
		m_Params._factors.useNormalMap = 1;
	}

	if (params.occulsionTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureMapType::Occulsion] = ResourcePool::Get()->GetTexture(params.occulsionTexture);
		m_Params._factors.useOculsionMap = 1;
	}

	if (params.emissiveTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureMapType::Emissive] = ResourcePool::Get()->GetTexture(params.emissiveTexture);
		m_Params._factors.useEmissiveMap = 1;
	}
	if (params.metalicRoughness.metallicRoughnessTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureMapType::MetallicRoughness] = ResourcePool::Get()->GetTexture(params.metalicRoughness.metallicRoughnessTexture);
		m_Params._factors.useSurfaceMap = 1;
	}
	if (params.normalTexture != UINT64_MAX)
	{
		m_Params._Maps[TextureMapType::SpecularGlossiness] = ResourcePool::Get()->GetTexture(params.specularGlossiness.specularGlossinessTexture);
		m_Params._factors.useSurfaceMap = 1;
	}
}
