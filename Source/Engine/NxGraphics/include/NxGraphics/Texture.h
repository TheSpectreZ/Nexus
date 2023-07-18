#pragma once
#include "Meshing.h"

namespace Nexus
{
	struct TextureSpecification
	{
		Extent extent;
		void* pixels;
		bool now = false;
		TextureType type;
		TextureUsage usage;
		TextureFormat format;
		uint32_t mipCount = 1;
	};

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual void PrepareForRender() = 0;
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