#pragma once
#include <unordered_map>

#include "NxCore/Base.h"
#include "Buffer.h"

namespace Nexus
{
	//class ResourcePool
	//{
	//public:
	//	void Clear();
	//
	//	void AllocateUniformBuffer(uint64_t hashId, uint32_t size);
	//	void DeallocateUniformBuffer(uint64_t hashId);
	//
	//	Ref<UniformBuffer> GetUniformBuffer(uint64_t hashId) { return m_UniformBuffers[hashId]; };
	//private:
	//	std::unordered_map<uint64_t, Ref<UniformBuffer>> m_UniformBuffers;
	//	std::unordered_map<uint64_t, bool> m_GrabageUniformBuffers;
	//};
	
	typedef uint64_t ResourceID;

	class ResourcePool
	{
		template<typename T>
		class RefPoolImpl
		{
		public:
			void Clear()
			{
				m_Pool.clear();
			}

			template<typename... Args>
			void Allocate(const ResourceID& Id,Args&&... args)
			{
				m_Pool[Id] = CreateRef<T>(std::forward<Args>(args)...);
			}

			void DeAllocate(const ResourceID& Id)
			{
				if (m_Pool.contains(Id))
					m_Pool.erase(Id);
			}
		private:
			std::unordered_map<ResourceID, Ref<T>> m_Pool;
		};
		
		RefPoolImpl<UniformBuffer> m_UniformBufferPool;
	public:
		template<typename T>
		void Clear();

		template<typename T,typename... Args>
		void Allocate(const ResourceID& Id,Args&&... args);

		template<typename T>
		void Deallocate(const ResourceID& Id);
	};
}
