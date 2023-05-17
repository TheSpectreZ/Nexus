#pragma once
#include "Assets/Asset.h"
#include "Assets/Importer/glTFImporter.h"
#include "glm/glm.hpp"

namespace Nexus
{ 
	struct CombinedImageSampler
	{
		UUID Image;
		UUID Sampler;
		uint8_t TexCoord;
	};

	struct MaterialCreateInfo
	{
		CombinedImageSampler albedo;
		CombinedImageSampler metallicRoughness;
		CombinedImageSampler normal;
		glm::vec4 albedoColor;
		float roughness, metalness;
		float useMR, useAlb, useNormal;
	};

	class Material : public Asset
	{
		friend class SceneBuildData;
	public:
		static Ref<Material> Create(const MaterialCreateInfo& Info);

		Material() = default;
		~Material() override = default;
	private:
		CombinedImageSampler m_AlbedoMap;
		CombinedImageSampler m_MetallicRoughnessMap;
		CombinedImageSampler m_Normal;

		float useAlbedo;
		float useMR;
		float useNormal;

		glm::vec4 m_AlbedoColor;
		float m_metalness;
		float m_roughness;
	};
}