#include "NxGraphics/ResourcePool.h"
#include "NxCore/Logger.h"

/*void Nexus::ResourcePool::Clear()
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

//		NEXUS_LOG("Vulkan", "Uniform Buffer Allocated : %i | %i", size, hashId);
		return;
	}

	// Fix This
	//m_UniformBuffers[hashId] = UniformBuffer::Create(size);
	//NEXUS_LOG("Vulkan", "Uniform Buffer Allocated : %i | %i", size, hashId);
}

void Nexus::ResourcePool::DeallocateUniformBuffer(uint64_t hashId)
{
	if (m_GrabageUniformBuffers.contains(hashId)) // Check Shader Resource Allocator
		return;

	m_GrabageUniformBuffers[hashId] = true;
	//NEXUS_LOG("Vulkan", "Uniform Buffer Deallocated : %i", hashId);
}
*/

template<>
void Nexus::ResourcePool::Clear<Nexus::UniformBuffer>()
{
	m_UniformBufferPool.Clear();
}

template<>
void Nexus::ResourcePool::Allocate<Nexus::UniformBuffer>(const ResourceID& Id,uint32_t&& args)
{
	m_UniformBufferPool.Allocate(Id, std::forward<uint32_t>(args));
}

template<>
void Nexus::ResourcePool::Deallocate<Nexus::UniformBuffer>(const ResourceID& Id)
{
	m_UniformBufferPool.DeAllocate(Id);
}