#pragma once
#include "RenderTypes.h"

namespace Nexus
{
	struct TextureCreateInfo
	{
		Extent extent;
		uint32_t channels;
		void* pixeldata;
	};

	class Texture
	{
	public:
		static Ref<Texture> LoadFromFile(const char* filepath);
		static Ref<Texture> Create(const TextureCreateInfo& Info);

		Texture() = default;
		virtual ~Texture() = default;
	};

	enum class SamplerFilter
	{
		Nearest, Linear
	};

	class Sampler
	{
	public:
		static Ref<Sampler> Create(SamplerFilter Near, SamplerFilter Far);
		
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}
