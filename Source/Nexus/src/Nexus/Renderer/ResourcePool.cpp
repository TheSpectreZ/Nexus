#include "nxpch.h"
#include "ResourcePool.h"

void Nexus::ResourcePool::Clear()
{
	m_UniformBuffers.clear();
	m_GrabageUniformBuffers.clear();
}

void Nexus::ResourcePool::AllocateUniformBuffer(uint64_t hashId,uint32_t size)
{
	if (!m_GrabageUniformBuffers.empty())
	{
		uint64_t Id = 0;
		for (auto& i : m_GrabageUniformBuffers)
		{
			Id = i.first;
		}

		m_GrabageUniformBuffers.erase(Id);

		auto nh = m_UniformBuffers.extract(Id);
		nh.key() = hashId;
		m_UniformBuffers.insert(std::move(nh));

		NEXUS_LOG_TRACE("Vulkan Uniform Buffer Allocated: {0}|{1}", size, hashId);
		return;
	}

	m_UniformBuffers[hashId] = UniformBuffer::Create(size);
	NEXUS_LOG_TRACE("Vulkan Uniform Buffer Allocated: {0}|{1}", size, hashId);
}

void Nexus::ResourcePool::DeallocateUniformBuffer(uint64_t hashId)
{
	if (m_GrabageUniformBuffers.contains(hashId)) // Check Shader Resource Allocator
		return;

	m_GrabageUniformBuffers[hashId] = true;
	NEXUS_LOG_TRACE("Vulkan Uniform Buffer Deallocated: {0}", hashId);
}
