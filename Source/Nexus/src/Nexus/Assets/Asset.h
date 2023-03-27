#pragma once
#include <cstdint>
#include <filesystem>

#include "Renderer/Mesh.h"

namespace Nexus
{
	typedef uint64_t AssetHandle;

	struct StaticMeshAsset
	{
		std::string Name;
		std::filesystem::path Path;

		Ref<StaticMesh> Mesh;
	};

}