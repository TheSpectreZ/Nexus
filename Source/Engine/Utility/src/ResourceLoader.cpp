#include "Utility/ResourceLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Nexus::Utility::ResourceLoader::Initialize()
{
}

void Nexus::Utility::ResourceLoader::LoadImage(Image* image, const std::string& filepath)
{
	int channels;
	image->pixels = stbi_load(filepath.c_str(), &image->width, &image->height, &channels, 4);
}

void Nexus::Utility::ResourceLoader::FreeImage(Image* image)
{
	stbi_image_free(image->pixels);
}

void Nexus::Utility::ResourceLoader::LoadImages(Image** pImages, int count, std::string* pFilepaths)
{
	int channels;
	for (int i = 0; i < count; i++)
	{
		pImages[i]->pixels = stbi_load(pFilepaths[i].c_str(), &pImages[i]->width, &pImages[i]->height, &channels, 4);
	}
}

void Nexus::Utility::ResourceLoader::FreeImages(Image** pImages, int count)
{
	for (int i = 0; i < count; i++)
	{
		stbi_image_free(pImages[i]->pixels);
	}
}
