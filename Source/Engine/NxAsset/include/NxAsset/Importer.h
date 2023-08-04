#pragma once
#include "NxCore/UUID.h"
#include "NxRenderEngine/Meshing.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	struct glTFImportSettings
	{
		std::string Name;
		AssetFilePath path;

		bool loadSkeleton = false;
		bool loadMaterials = false;
		bool loadAnimations = false;
	};

	class NEXUS_ASSET_API Importer
	{
	public:
		static bool ImportglTF(const AssetFilePath& filepath, const glTFImportSettings& settings);
		static bool ImportImage(const AssetFilePath& filepath, const std::string& dstFolder, uint32_t samplerHash);
		
		static void Mat();
	};

	struct MeshAssetSpecification
	{
		UUID mesh = UINT64_MAX;
		UUID skeleton = UINT64_MAX;
		std::unordered_map<uint32_t, UUID> materials;
	};

	class NEXUS_ASSET_API Loader
	{
	public:
		static bool LoadMeshAsset(const AssetFilePath& filepath, MeshAssetSpecification* specs);
	};
}
