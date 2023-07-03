#pragma once
#include "AssetSpecifications.h"

namespace Nexus
{
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