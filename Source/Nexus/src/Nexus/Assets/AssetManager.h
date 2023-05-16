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

		template<typename T,typename... Args>
		static std::pair<Ref<T>, UUID> Load(Args&&... args)
		{
			// Make Sure that Asset Has a Static Create Function
			Ref<Asset> asset = T::Create(std::forward<Args>(args)...);

			UUID Id = CreateUUID();
			asset->m_Id = Id;
			
			s_Instance->m_Assets[Id] = asset;
			return { DynamicPointerCast<T>(asset),Id };
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


