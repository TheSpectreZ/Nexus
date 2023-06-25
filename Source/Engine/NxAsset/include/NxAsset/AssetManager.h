#pragma once
#include <filesystem>
#include "NxCore/Base.h"
#include "NxCore/UUID.h"
#include "Asset.h"

namespace Nexus
{
	class AssetManager
	{
		static AssetManager* s_Instance;
	public:
		static void Initialize(bool loadDefaultAssets);
		static void Shutdown();

		template<typename T,typename... Args>
		static std::pair<Ref<T>, UUID> Load(Args&&... args)
		{
			Ref<Asset> asset = T::Create(std::forward<Args>(args)...);

			s_Instance->m_Assets[asset->GetID()] = asset;
			return { DynamicPointerCast<T>(asset),asset->GetID() };
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


