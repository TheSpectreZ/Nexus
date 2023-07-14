#include "NxRenderEngine/ResourcePool.h"

#include "NxCore/Assertion.h"

Nexus::ResourcePool* Nexus::ResourcePool::s_Instance = nullptr;

void Nexus::ResourcePool::Initialize()
{
	s_Instance = new ResourcePool;
}

void Nexus::ResourcePool::Shutdown()
{
	s_Instance->m_Textures.clear();
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

RESOURCE_ALLOC_METHOD_GI_IMP(Texture, TextureSpecification, m_Textures)
RESOURCE_ALLOC_METHOD_REF_IMP(RenderableMesh, Meshing::Mesh, m_RenderableMeshes)

Nexus::Ref<Nexus::RenderableMaterial> Nexus::ResourcePool::AllocateRenderableMaterial(const Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures, UUID HashId)
{
	if (!m_RenderableMaterials.contains(HashId))
	{
		std::unordered_map<TextureType, uint32_t> Samplers;

		TextureSpecification ts{};
		SamplerSpecification ss{};

		if (textures.contains((uint8_t)TextureType::Albedo))
		{
			Samplers[TextureType::Albedo] = textures[(uint32_t)TextureType::Albedo].samplerHash;

			ts.image = textures[(uint8_t)TextureType::Albedo].image;
			if (specs.specularGlossiness.support)
				AllocateTexture(ts, specs.specularGlossiness.albedoTexture);
			else
				AllocateTexture(ts, specs.metalicRoughness.albedoTexture);
		}

		if (textures.contains((uint8_t)TextureType::Emissive))
		{
			Samplers[TextureType::Emissive] = textures[(uint32_t)TextureType::Emissive].samplerHash;
			
			ts.image = textures[(uint8_t)TextureType::Emissive].image;
			AllocateTexture(ts, specs.emissiveTexture);
		}

		if (textures.contains((uint8_t)TextureType::Normal))
		{
			Samplers[TextureType::Normal] = textures[(uint32_t)TextureType::Normal].samplerHash;

			ts.image = textures[(uint8_t)TextureType::Normal].image;
			AllocateTexture(ts, specs.normalTexture);
		}

		if (textures.contains((uint8_t)TextureType::Occulsion))
		{
			Samplers[TextureType::Occulsion] = textures[(uint32_t)TextureType::Occulsion].samplerHash;
			
			ts.image = textures[(uint8_t)TextureType::Occulsion].image;
			AllocateTexture(ts, specs.occulsionTexture);
		}

		if (textures.contains((uint8_t)TextureType::SpecularGlossiness))
		{
			Samplers[TextureType::SpecularGlossiness] = textures[(uint32_t)TextureType::SpecularGlossiness].samplerHash;

			ts.image = textures[(uint8_t)TextureType::SpecularGlossiness].image;
			AllocateTexture(ts, specs.specularGlossiness.specularGlossinessTexture);
		}

		if (textures.contains((uint8_t)TextureType::MetallicRoughness))
		{
			Samplers[TextureType::MetallicRoughness] = textures[(uint32_t)TextureType::MetallicRoughness].samplerHash;

			ts.image = textures[(uint8_t)TextureType::MetallicRoughness].image;
			AllocateTexture(ts, specs.metalicRoughness.metallicRoughnessTexture);
		}

		m_RenderableMaterials[HashId] = CreateRef<RenderableMaterial>(specs, Samplers);
	}

	return m_RenderableMaterials[HashId];
}

void Nexus::ResourcePool::DeallocateRenderableMaterial(UUID HashId)
{
	if (m_RenderableMaterials.contains(HashId)) m_RenderableMaterials.erase(HashId);
}