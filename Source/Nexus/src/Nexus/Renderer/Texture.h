#pragma once
#include "RenderTypes.h"
#include "Assets/Asset.h"

namespace Nexus
{
	struct TextureCreateInfo
	{
		Extent extent;
		uint32_t channels;
		void* pixeldata;
	};

	class Texture : public Asset
	{
	public:
		static Ref<Texture> Create(const std::string& file);
		static Ref<Texture> Create(const TextureCreateInfo& Info);

		Texture() = default;
		Texture(const Texture&) = default;
		virtual ~Texture() = default;
	};

	class Sampler : public Asset
	{
	public:
		static Ref<Sampler> Create(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W);
		
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}
