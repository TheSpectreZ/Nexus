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
		glm::vec4 albedoColor;
		float roughness, metalness;
		float useMR, useAlb;
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

		float useAlbedo;
		float useMR;

		glm::vec4 m_AlbedoColor;
		float m_metalness;
		float m_roughness;
	};
}