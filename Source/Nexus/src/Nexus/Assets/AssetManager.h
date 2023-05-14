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
		static UUID Emplace(Ref<Asset> asset)
		{
			s_Instance->m_Assets[asset->GetID()] = asset;
			return asset->GetID();
		}

		template<typename T>
		static Ref<T> Get(UUID handle)
		{
			return DynamicPointerCast<T>(s_Instance->m_Assets[handle]);
		}

		static bool Has(UUID handle)
		{
			return s_Instance->m_Assets.contains(handle);
		}

		static void Remove(UUID handle)
		{
			if (s_Instance->m_Assets.find(handle) != s_Instance->m_Assets.end())
			{
				s_Instance->m_Assets.erase(handle);
			}
		}
	private:
		std::unordered_map<UUID, Ref<Asset>> m_Assets;
	};
	
}


