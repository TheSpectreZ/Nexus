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
	};

	struct MaterialCreateInfo
	{
		CombinedImageSampler albedo;
		glm::vec4 albedoColor;
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
		glm::vec4 m_AlbedoColor;
	};
}