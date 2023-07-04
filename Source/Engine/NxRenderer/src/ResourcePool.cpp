#include "NxRenderer/ResourcePool.h"

#include "NxCore/Assertion.h"

Nexus::ResourcePool* Nexus::ResourcePool::s_Instance = nullptr;

void Nexus::ResourcePool::Initialize()
{
	s_Instance = new ResourcePool;
}

void Nexus::ResourcePool::Shutdown()
{
	s_Instance->m_RenderableMeshes.clear();
	s_Instance->m_UniformBuffers.clear();
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

Nexus::Ref<Nexus::RenderableMesh> Nexus::ResourcePool::AllocateRenderableMesh(const RenderableMeshSpecification& specs, UUID HashID)
{
	if (!m_RenderableMeshes.contains(HashID))
		m_RenderableMeshes[HashID] = CreateRef<RenderableMesh>(specs);

	return m_RenderableMeshes[HashID];
}

void Nexus::ResourcePool::DeallocateRenderableMesh(UUID HashID)
{
	if (m_RenderableMeshes.contains(HashID))
		m_RenderableMeshes.erase(HashID);
}

Nexus::Ref<Nexus::Texture> Nexus::ResourcePool::AllocateTexture(const TextureSpecification& specs, UUID HashID)
{
	if (!m_Textures.contains(HashID))
		m_Textures[HashID] = GraphicsInterface::CreateTexture(specs);

	return m_Textures[HashID];
}

void Nexus::ResourcePool::DeallocateTexture(UUID HashID)
{
	if (m_Textures.contains(HashID))
		m_Textures.erase(HashID);
}
