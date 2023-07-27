#include "NxRenderEngine/ResourcePool.h"

#include "NxCore/Assertion.h"

Nexus::ResourcePool* Nexus::ResourcePool::s_Instance = nullptr;

Nexus::ResourcePool::ResourcePool()
{
	s_Instance = this;
}

Nexus::ResourcePool::~ResourcePool()
{
	s_Instance = nullptr;
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

Nexus::Ref<Nexus::RenderableMaterial> Nexus::ResourcePool::AllocMaterialImpl(Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures)
{
	std::unordered_map<TextureMapType, uint32_t> Samplers;

	TextureSpecification ts{};
	ts.format = TextureFormat::RGBA8_SRGB;
	ts.type = TextureType::TwoDim;
	ts.usage = TextureUsage::ShaderSampled;

	if (textures.contains((uint8_t)TextureMapType::Albedo))
	{
		Samplers[TextureMapType::Albedo] = textures.at((uint32_t)TextureMapType::Albedo).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::Albedo).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		if (specs.specularGlossiness.support)
			AllocateAsset<Texture>(specs.specularGlossiness.albedoTexture, ts);
		else
			AllocateAsset<Texture>(specs.metalicRoughness.albedoTexture, ts);
	}

	if (textures.contains((uint8_t)TextureMapType::Emissive))
	{
		Samplers[TextureMapType::Emissive] = textures.at((uint32_t)TextureMapType::Emissive).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::Emissive).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		AllocateAsset<Texture>(specs.emissiveTexture, ts);
	}

	if (textures.contains((uint8_t)TextureMapType::Normal))
	{
		Samplers[TextureMapType::Normal] = textures.at((uint32_t)TextureMapType::Normal).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::Normal).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		AllocateAsset<Texture>(specs.normalTexture, ts);
	}

	if (textures.contains((uint8_t)TextureMapType::Occulsion))
	{
		Samplers[TextureMapType::Occulsion] = textures.at((uint32_t)TextureMapType::Occulsion).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::Occulsion).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		AllocateAsset<Texture>(specs.occulsionTexture, ts);
	}

	if (textures.contains((uint8_t)TextureMapType::SpecularGlossiness))
	{
		Samplers[TextureMapType::SpecularGlossiness] = textures.at((uint32_t)TextureMapType::SpecularGlossiness).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::SpecularGlossiness).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		AllocateAsset<Texture>(specs.specularGlossiness.specularGlossinessTexture, ts);
	}

	if (textures.contains((uint8_t)TextureMapType::MetallicRoughness))
	{
		Samplers[TextureMapType::MetallicRoughness] = textures.at((uint32_t)TextureMapType::MetallicRoughness).samplerHash;

		auto& albedo = textures.at((uint8_t)TextureMapType::MetallicRoughness).image;
		ts.extent = { albedo.width,albedo.height };
		ts.pixels = albedo.pixels.data();

		AllocateAsset<Texture>(specs.metalicRoughness.metallicRoughnessTexture, ts);
	}

	MaterialParameters mParams;
	{
		mParams._Samplers = Samplers;

		mParams._factors.albedo = specs.specularGlossiness.support ? specs.specularGlossiness.aldeboColor : specs.metalicRoughness.albedoColor;
		mParams._factors.glossiness = specs.specularGlossiness.glossiness;
		mParams._factors.metalness = specs.metalicRoughness.metallic;
		mParams._factors.roughness = specs.metalicRoughness.roughness;
		mParams._factors.specular = specs.specularGlossiness.specular;
		mParams._factors.emissive = specs.emissiveColor;

		if (specs.specularGlossiness.support)
		{
			mParams._factors.pbrType = 2;
			if (specs.specularGlossiness.albedoTexture != UINT64_MAX)
			{
				mParams._Maps[TextureMapType::Albedo] = GetAsset<Texture>(specs.specularGlossiness.albedoTexture);
				mParams._factors.useBaseColorMap = 1;
			}
		}
		else
		{
			mParams._factors.pbrType = 1;
			if (specs.metalicRoughness.albedoTexture != UINT64_MAX)
			{
				mParams._Maps[TextureMapType::Albedo] = GetAsset<Texture>(specs.metalicRoughness.albedoTexture);
				mParams._factors.useBaseColorMap = 1;
			}
		}

		if (specs.normalTexture != UINT64_MAX)
		{
			mParams._Maps[TextureMapType::Normal] = GetAsset<Texture>(specs.normalTexture);
			mParams._factors.useNormalMap = 1;
		}

		if (specs.occulsionTexture != UINT64_MAX)
		{
			mParams._Maps[TextureMapType::Occulsion] = GetAsset<Texture>(specs.occulsionTexture);
			mParams._factors.useOculsionMap = 1;
		}

		if (specs.emissiveTexture != UINT64_MAX)
		{
			mParams._Maps[TextureMapType::Emissive] = GetAsset<Texture>(specs.emissiveTexture);
			mParams._factors.useEmissiveMap = 1;
		}
		if (specs.metalicRoughness.metallicRoughnessTexture != UINT64_MAX)
		{
			mParams._Maps[TextureMapType::MetallicRoughness] = GetAsset<Texture>(specs.metalicRoughness.metallicRoughnessTexture);
			mParams._factors.useSurfaceMap = 1;
		}
		if (specs.normalTexture != UINT64_MAX)
		{
			mParams._Maps[TextureMapType::SpecularGlossiness] = GetAsset<Texture>(specs.specularGlossiness.specularGlossinessTexture);
			mParams._factors.useSurfaceMap = 1;
		}
	}

	return CreateRef<RenderableMaterial>(mParams);
}

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