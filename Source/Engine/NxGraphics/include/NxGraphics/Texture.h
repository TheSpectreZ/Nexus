#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	struct TextureCreateInfo
	{
		Extent extent;
		void* pixeldata;
	};

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
	};

	class Sampler
	{
	public:
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}