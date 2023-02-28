#pragma once
#include "Build.h"
#include <string>

#undef LoadImage

namespace Nexus::Utility
{
	struct NEXUS_UTILITY_API Image
	{
		unsigned char* pixels;
		int width, height, channels;
	};

	class NEXUS_UTILITY_API ResourceLoader
	{
	public:
		
		static void Initialize();
		
		static void LoadImage(Image* image, const std::string& filepath, int channels);
		static void FreeImage(Image* image);
		
		static void LoadImages(Image** pImages, int count, std::string* pFilepaths, int channels);
		static void FreeImages(Image** pImages, int count);
	};
}

