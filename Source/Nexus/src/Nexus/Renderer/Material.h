#pragma once
#include "Assets/AssetManager.h"

namespace Nexus
{
	struct MaterialSpecifications
	{
		std::string AlbedoPath;
	};

	struct Material
	{
	public:
		static Ref<Material> Create(const std::string& filepath);

		Material() = default;
		~Material() = default;

		Ref<Texture> GetAlebdoTexture();
	private:
		static MaterialSpecifications Deserialize(const std::string& path);

		AssetHandle m_Alebdo;
	};
}
