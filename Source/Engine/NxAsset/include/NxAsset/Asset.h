#pragma once
#include <filesystem>
#include <fstream>
#include "NxCore/UUID.h"

#include "NxGraphics/Meshing.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	typedef NEXUS_ASSET_API std::filesystem::path AssetFilePath;

	namespace Importer
	{
		bool NEXUS_ASSET_API ImportGLTF(const AssetFilePath& path, const AssetFilePath& destFolder, const std::string& Name);

		std::pair<bool, UUID> NEXUS_ASSET_API LoadMesh(const AssetFilePath& path, std::vector<Meshing::Mesh>& meshes);
	}
}