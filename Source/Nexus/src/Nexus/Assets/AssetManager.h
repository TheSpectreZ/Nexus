#pragma once
#include <filesystem>

#include "Mesh.h"

namespace Nexus
{
	typedef uint64_t AssetHandle;

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
		static Ref<T> Get(AssetHandle handle);

		template<typename T>
		static bool Has(AssetHandle handle);

		template<typename T>
		static void Remove(AssetHandle handle);
	private:
		std::unordered_map<AssetHandle, Ref<StaticMesh>> m_Meshes;
	};
	
}


