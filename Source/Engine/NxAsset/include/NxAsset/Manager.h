#pragma once
#include "NxCore/Base.h"
#include "NxCore/UUID.h"
#include "NxCore/Asset.h"
#include "NxCore/Registry.h"
#include "NxRenderEngine/RenderableAsset.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_ASSET_API AssetManager
	{
		static AssetManager* s_Instance;
	public:
		static AssetManager* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		template<typename T,typename... Args>
		inline Ref<T> Load(UUID Id,Args&&... args)
		{
			if (!T::ContainsInPool(Id))
				T::AddToPool(Id, std::forward<Args>(args)...);
				
			return T::GetFromPool(Id);
		}

		template<typename T>
		inline Ref<T> Get(UUID Id)
		{
			return T::GetFromPool(Id);
		}

		template<typename T>
		inline void Delete(UUID Id)
		{
			T::EraseFromPool(Id);
		}
	private:
		AssetRegistry m_registry;
	};
}
