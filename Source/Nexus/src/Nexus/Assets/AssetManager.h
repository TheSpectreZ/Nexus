#pragma once
#include <filesystem>
#include "Asset.h"

namespace Nexus
{
	class AssetManager
	{
		static AssetManager* s_Instance;
	public:
		static void Initialize();
		static void Shutdown();

		template<typename T>
		static UUID LoadFromFile(const std::filesystem::path& path);

		template<typename T>
		static T& Get(UUID handle);

		template<typename T>
		static bool Has(UUID handle);

		template<typename T>
		static void Remove(UUID handle);
	private:
		std::unordered_map<std::string, UUID> m_UUIDCache;

		std::unordered_map<UUID, StaticMeshAsset> m_StaticMeshes;
		std::unordered_map<UUID, TextureAsset> m_Textures;
	};
	
}


