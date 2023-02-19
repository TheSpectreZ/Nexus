#pragma once
#include "Build.h"
#include <string>

namespace Nexus::Utility
{
	struct Image
	{
		unsigned char* pixels;
		int width, height;
	};

	class ResourceLoader
	{
	public:
		void Initialize();

		void LoadImage(Image* image, const std::string& filepath);
		void FreeImage(Image* image);

		void LoadImages(Image** pImages, int count, std::string* pFilepaths);
		void FreeImages(Image** pImages, int count);
	};
}

