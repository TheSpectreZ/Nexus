#pragma once
#include <unordered_map>

#include "NxCore/Base.h"
#include "NxCore/UUID.h"
#include "Asset.h"

namespace Nexus::Module
{
	struct NEXUS_ASSET_API AssetLoadResult
	{
		bool success = false;
		UUID id = UUID(true);
		Ref<Asset> asset = nullptr;
	};

	class NEXUS_ASSET_API AssetManager
	{
		static AssetManager* s_Instance;
	public:
		static AssetManager* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		bool Import(AssetType type,const AssetFilePath& Source, const AssetFilePath& Asset, const AssetFilePath& Bin);
		
		AssetLoadResult Load(AssetType type,const AssetFilePath& Asset);

		template<typename T>
		Ref<T> Retrive(AssetType type, UUID Id);
	private:
		std::unordered_map<UUID, Ref<Asset>> m_Assets;
	};

	template<typename T>
	Ref<T> AssetManager::Retrive(AssetType type, UUID Id)
	{
		if (!m_Assets.contains(Id))
			return nullptr;

		if (m_Assets[Id]->GetType() != type)
			return nullptr;

		return DynamicPointerCast<T>(m_Assets[Id]);
	}
}
