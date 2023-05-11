#pragma once
#include <filesystem>
#include "Asset.h"

namespace Nexus
{
	class AssetManager
	{
		static AssetManager* s_Instance;
		static AssetHandle CreateAssetHandle();
	public:
		static void Initialize();
		static void Shutdown();

		template<typename T>
		static AssetHandle LoadFromFile(const std::filesystem::path& path);

		template<typename T>
		static T& Get(AssetHandle handle);

		template<typename T>
		static bool Has(AssetHandle handle);

		template<typename T>
		static void Remove(AssetHandle handle);
	private:
		std::unordered_map<std::string, AssetHandle> m_AssetHandleCache;

		std::unordered_map<AssetHandle, StaticMeshAsset> m_StaticMeshes;
		std::unordered_map<AssetHandle, TextureAsset> m_Textures;
	};
	
}


