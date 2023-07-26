#include "NxRenderEngine/ResourcePool.h"

#include "NxCore/Assertion.h"

Nexus::ResourcePool* Nexus::ResourcePool::s_Instance = nullptr;

void Nexus::ResourcePool::Initialize()
{
	s_Instance = new ResourcePool;
}

void Nexus::ResourcePool::Shutdown()
{
	delete s_Instance;
}

Nexus::Ref<Nexus::Buffer> Nexus::ResourcePool::AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle)
{
	if (!m_UniformBuffers.contains(handle.hashId))
	{
		ShaderResouceHeapLayoutBinding* binding = nullptr;
		shader->GetShaderResourceHeapLayoutBinding(binding, handle.set, handle.binding);

		NEXUS_BREAK_ASSERT((binding == nullptr), "Shader Set-Binding Doesn't Exists: %i-%i", handle.set, handle.binding);

		BufferSpecification specs{};
		specs.type = BufferType::Uniform;
		specs.size = binding->bufferSize;
		specs.data = nullptr;
		specs.cpuMemory = true;

		m_UniformBuffers[handle.hashId] = GraphicsInterface::CreateBuffer(specs);
	}

	return m_UniformBuffers[handle.hashId];
}

void Nexus::ResourcePool::DeallocateUniformBuffer(UUID HashID)
{
	if (m_UniformBuffers.contains(HashID))
		m_UniformBuffers.erase(HashID);
}

Nexus::Ref<Nexus::Sampler> Nexus::ResourcePool::GetSampler(uint32_t HashId)
{
	if (m_Samplers.contains(HashId))
		return m_Samplers[HashId];

	SamplerSpecification specs{};
	specs.sampler.ResolveHash(HashId);

	m_Samplers[HashId] = GraphicsInterface::CreateSampler(specs);

	return m_Samplers[HashId];
}

Nexus::Ref<Nexus::Sampler> Nexus::ResourcePool::GetSampler(const SamplerSpecification& specs)
{
	auto samp = specs.sampler;
	uint32_t hashId = samp.GetHash();

	if (m_Samplers.contains(hashId))
		return m_Samplers[hashId];

	m_Samplers[hashId] = GraphicsInterface::CreateSampler(specs);
	return m_Samplers[hashId];
}

#define RESOURCE_ALLOC_METHOD_GI_IMP(Resource,Specs,m_Member)\
Nexus::Ref<Nexus::##Resource> Nexus::ResourcePool::Allocate##Resource(const Specs& specs,UUID HashId)\
{\
	if(!m_Member.contains(HashId))\
		m_Member[HashId] = GraphicsInterface::Create##Resource(specs);\
	return m_Member[HashId];\
}\
void Nexus::ResourcePool::Deallocate##Resource(UUID HashId)\
{\
	if(m_Member.contains(HashId))\
		m_Member.erase(HashId);\
}\

#define RESOURCE_ALLOC_METHOD_REF_IMP(Resource,Specs,m_Member)\
Nexus::Ref<Nexus::##Resource> Nexus::ResourcePool::Allocate##Resource(const Specs& specs,UUID HashId)\
{\
	if(!m_Member.contains(HashId))\
		m_Member[HashId] = CreateRef<Resource>(specs);\
	return m_Member[HashId];\
}\
void Nexus::ResourcePool::Deallocate##Resource(UUID HashId)\
{\
	if(m_Member.contains(HashId))\
		m_Member.erase(HashId);\
}\

Nexus::Ref<Nexus::Environment> Nexus::ResourcePool::AllocateEnvironment(UUID HashID)
{
	if (!m_Environments.contains(HashID))
		m_Environments[HashID] = CreateRef<Environment>();

	return m_Environments[HashID];
}

void Nexus::ResourcePool::DeallocateEnvironment(UUID HashID)
{
	if (m_Environments.contains(HashID)) m_Environments.erase(HashID);
}
