#include "NxRenderer/ResourcePool.h"

#include "NxCore/Assertion.h"

void Nexus::ResourcePool::Initialize()
{
}

void Nexus::ResourcePool::Shutdown()
{
	m_UniformBuffers.clear();
}

void Nexus::ResourcePool::AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle)
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

void Nexus::ResourcePool::DeallocateUniformBuffer(UUID HashID)
{
	m_UniformBuffers[HashID].reset();
}

void Nexus::ResourcePool::UpdateUniformBuffer(UUID HashID, void* Data)
{
	m_UniformBuffers[HashID]->Update(Data);
}
