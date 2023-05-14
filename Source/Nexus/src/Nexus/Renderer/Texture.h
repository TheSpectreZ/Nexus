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

		Texture() = default;
		Texture(const Texture&) = default;
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
