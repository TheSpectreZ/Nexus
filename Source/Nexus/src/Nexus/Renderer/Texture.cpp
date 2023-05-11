#include "nxpch.h"
#include "Texture.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Nexus::Ref<Nexus::Texture> Nexus::Texture::LoadFromFile(const char* filepath)
{
	TextureCreateInfo Info{};
	
	int w, h, c;
	Info.pixeldata = stbi_load(filepath, &w, &h, &c, 4);

	Info.channels = c;
	Info.extent = { (uint32_t)w,(uint32_t)h };

	Nexus::Ref<Nexus::Texture> ref = Create(Info);
	stbi_image_free(Info.pixeldata);

	return ref;
}

Nexus::Ref<Nexus::Texture> Nexus::Texture::Create(const TextureCreateInfo& Info)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanTexture>(Info);
		case RenderAPIType::NONE: return  nullptr;
		default: return nullptr;
	}
}
