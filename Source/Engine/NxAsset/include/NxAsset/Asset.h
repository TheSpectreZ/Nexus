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
		bool NEXUS_ASSET_API ImportImage(const AssetFilePath& path, const AssetFilePath& destFolder, const std::string& Name);

		std::pair<bool, UUID> NEXUS_ASSET_API LoadMesh(const AssetFilePath& path, Meshing::Mesh& meshes, std::unordered_map<uint32_t, std::string>* materialTablePtr);
		std::pair<bool, UUID> NEXUS_ASSET_API LoadImage(const AssetFilePath& path, Meshing::Image& image);
		std::pair<bool, UUID> NEXUS_ASSET_API LoadTexture(const AssetFilePath& path, Meshing::Texture& texture);
		std::pair<bool, UUID> NEXUS_ASSET_API LoadMaterial(const AssetFilePath& path, Meshing::Material& material, std::unordered_map<uint8_t, Meshing::Texture>& textures);
	}
}