#pragma once
#include "RenderTypes.h"
#include "Assets/Asset.h"

namespace Nexus
{
	struct TextureCreateInfo
	{
		std::string UniqueNameOrFilepath;
		Extent extent;
		uint32_t channels;
		void* pixeldata;
	};

	class Texture : public Asset
	{
		friend class AssetManager;
		static std::unordered_map<std::string, UUID> s_LoadedTextures;
	public:
		static Ref<Texture> Create(const std::string& file);
		static Ref<Texture> Create(const TextureCreateInfo& Info);

		Texture() = default;
		Texture(const Texture&) = default;
		virtual ~Texture() = default;
	};

	std::string MakeSamplername(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W);

	class Sampler : public Asset
	{
		friend class AssetManager;
		static std::unordered_map<std::string, UUID> s_LoadedSamplers;
	public:
		static Ref<Sampler> Create(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W);
		
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}
