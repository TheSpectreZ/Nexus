#pragma once
#include <filesystem>
#include <unordered_map>
#include "UUID.h"

#ifdef NEXUS_CORE_SHARED_BUILD
#define NEXUS_CORE_API __declspec(dllexport)
#else
#define NEXUS_CORE_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD

namespace Nexus
{
	class NEXUS_CORE_API AssetRegistry
	{
		static AssetRegistry* s_Instance;
	public:
		static AssetRegistry* Get() { return s_Instance; }
		
		AssetRegistry();
		~AssetRegistry();

		void SetProjectContext(const std::filesystem::path& projectDirectory);
		
		UUID Register(const std::filesystem::path& path);
		std::filesystem::path LookUp(UUID Id);
		UUID LookUp(const std::filesystem::path& path);
	private:
		void Serialize();
		void Deserialize();

		void RegisterEngineAssets();

		std::filesystem::path m_RegistryPath;
		std::unordered_map<UUID, std::filesystem::path> m_IdRegistry;
		std::unordered_map<std::filesystem::path, UUID> m_PathRegistry;
	};
}

