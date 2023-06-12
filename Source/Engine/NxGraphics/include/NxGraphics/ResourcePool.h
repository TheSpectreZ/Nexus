#pragma once
#include "NxCore/Base.h"
#include "Buffer.h"

namespace Nexus
{
	class ResourcePool
	{
	public:
		void Clear();

		void AllocateUniformBuffer(uint64_t hashId, uint32_t size);
		void DeallocateUniformBuffer(uint64_t hashId);

		Ref<UniformBuffer> GetUniformBuffer(uint64_t hashId) { return m_UniformBuffers[hashId]; };
	private:
		std::unordered_map<uint64_t, Ref<UniformBuffer>> m_UniformBuffers;
		std::unordered_map<uint64_t, bool> m_GrabageUniformBuffers;
	};
}
