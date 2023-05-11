#pragma once
#include <cstdint>
#include <filesystem>

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

namespace Nexus
{
	typedef uint64_t AssetHandle;

	struct StaticMeshAsset
	{
		std::string Name;
		std::filesystem::path Path;

		Ref<StaticMesh> Mesh;
	};

	struct TextureAsset
	{
		std::string Name;
		std::filesystem::path Path;

		Ref<Texture> Texture;
	};
}