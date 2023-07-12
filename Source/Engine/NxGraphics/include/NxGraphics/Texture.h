#pragma once
#include "Meshing.h"

namespace Nexus
{
	struct TextureSpecification
	{
		Meshing::Image image;
	};

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
	};

	struct SamplerSpecification
	{
		Meshing::Sampler sampler;
	};

	class Sampler
	{
	public:
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}