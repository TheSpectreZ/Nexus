#include "nxpch.h"
#include "Texture.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkTexture.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Nexus::Ref<Nexus::Texture> Nexus::Texture::Create(const std::string& filepath)
{
	TextureCreateInfo Info{};
	
	int w, h, c;
	Info.pixeldata = stbi_load(filepath.c_str(), &w, &h, &c, 4);

	Info.channels = c;
	Info.extent = { (uint32_t)w,(uint32_t)h };

	Ref<Texture> tex;

	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: tex = CreateRef<VulkanTexture>(Info); break;
		case RenderAPIType::NONE:break;
		default: break;
	}

	stbi_image_free(Info.pixeldata);
	return tex;
}

Nexus::Ref<Nexus::Texture> Nexus::Texture::Create(const TextureCreateInfo& Info)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanTexture>(Info); break;
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::Ref<Nexus::Sampler> Nexus::Sampler::Create(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W)
{
	switch (RenderAPI::GetCurrentAPI())
	{
	case RenderAPIType::VULKAN: return CreateRef<VulkanSampler>(Near, Far, U, V, W);
	case RenderAPIType::NONE: return  nullptr;
	default: return nullptr;
	}
}
