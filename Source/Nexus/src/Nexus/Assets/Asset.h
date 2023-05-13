#pragma once
#include <filesystem>
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

namespace Nexus
{
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