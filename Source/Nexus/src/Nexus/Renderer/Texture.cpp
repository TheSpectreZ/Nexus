#include "nxpch.h"
#include "Texture.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkTexture.h"
#include "Assets/AssetManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_map<std::string, Nexus::UUID> Nexus::Texture::s_LoadedTextures;
std::unordered_map<std::string, Nexus::UUID> Nexus::Sampler::s_LoadedSamplers;

Nexus::Ref<Nexus::Texture> Nexus::Texture::Create(const std::string& filepath)
{
	TextureCreateInfo Info{};
	
	int w, h, c;
	Info.pixeldata = stbi_load(filepath.c_str(), &w, &h, &c, 4);
	Info.UniqueNameOrFilepath = filepath;
	Info.channels = c;
	Info.extent = { (uint32_t)w,(uint32_t)h };

	Ref<Texture> tex = Create(Info);

	stbi_image_free(Info.pixeldata);
	return tex;
}

Nexus::Ref<Nexus::Texture> Nexus::Texture::Create(const TextureCreateInfo& Info)
{
	if (s_LoadedTextures.contains(Info.UniqueNameOrFilepath))
	{
		return AssetManager::Get<Texture>(s_LoadedTextures[Info.UniqueNameOrFilepath]);
	}

	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN:
		{
			Ref<Texture> ref = CreateRef<VulkanTexture>(Info);
			ref->m_Id = CreateUUID();
			ref->m_Path = Info.UniqueNameOrFilepath;

			s_LoadedTextures[Info.UniqueNameOrFilepath] = ref->m_Id;

			return ref;
		}
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::Ref<Nexus::Sampler> Nexus::Sampler::Create(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W)
{
	std::string name = MakeSamplername(Near, Far, U, V, W);
	if (s_LoadedSamplers.contains(name))
	{
		return AssetManager::Get<Sampler>(s_LoadedSamplers[name]);
	}

	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN:
		{
			Ref<Sampler> ref = CreateRef<VulkanSampler>(Near, Far, U, V, W);
			ref->m_Id = CreateUUID();
			ref->m_Path = name;

			s_LoadedSamplers[name] = ref->m_Id;

			return ref;
		}
		case RenderAPIType::NONE: return  nullptr;
		default: return nullptr;
	}
}

std::string Nexus::MakeSamplername(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W)
{
	std::stringstream ss;
	ss << (uint32_t)Near << (uint32_t)Far << (uint32_t)U << (uint32_t)V << (uint32_t)W;
	return ss.str();
}
