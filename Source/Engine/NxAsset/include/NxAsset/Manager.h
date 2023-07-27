#pragma once
#include "NxCore/Base.h"
#include "NxCore/UUID.h"
#include "NxCore/Object.h"

#include "NxRenderEngine/ResourcePool.h"
#include <unordered_map>

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif

namespace Nexus::Module
{
	class NEXUS_ASSET_API AssetManager
	{
		enum : uint8_t
		{
			assetType_None = 0,
			assetType_Mesh = 1,
			assetType_Material = 2,
			assetType_Texture = 3,
		};

		static AssetManager* s_Instance;
	public:
		static AssetManager* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		template<typename T,typename... Args>
		inline Ref<T> Allocate(UUID Id,Args&&... args)
		{
			return m_RenderEnginePool->AllocateAsset<T>(Id, std::forward<Args>(args)...);
		}

		template<typename T>
		inline Ref<T> Get(UUID Id)
		{
			return m_RenderEnginePool->AllocateAsset<T>(Id);
		}

		inline void Free(UUID Id)
		{
			m_RenderEnginePool->DeallocateAsset(Id);
		}

	private:
		Ref<ResourcePool> m_RenderEnginePool;
	};
}
