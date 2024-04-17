#pragma once
#include "NxCore/Asset.h"	
#include "TypeImpls.h"

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

	class NEXUS_GRAPHICS_API Texture 
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual void PrepareForRender() {};
	};

	struct SamplerSpecification
	{
		SamplerFilter Near;
		SamplerFilter Far;
		SamplerWrapMode U, V, W;

		uint32_t GetHash() const
		{
			uint32_t Id = 0;
			Id += (uint32_t)Far * 1;
			Id += (uint32_t)Near * 10;
			Id += (uint32_t)U * 100;
			Id += (uint32_t)V * 1000;
			Id += (uint32_t)W * 10000;

			return Id;
		}

		void ResolveHash(uint32_t HashId)
		{
			Far = (SamplerFilter)((HashId / 1) % 10);
			Near = (SamplerFilter)((HashId / 10) % 10);
			U = (SamplerWrapMode)((HashId / 100) % 10);
			V = (SamplerWrapMode)((HashId / 1000) % 10);
			W = (SamplerWrapMode)((HashId / 10000) % 10);
		};
	};

	class NEXUS_GRAPHICS_API Sampler
	{
	public:
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}