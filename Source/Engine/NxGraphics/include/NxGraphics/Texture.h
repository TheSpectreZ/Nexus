#pragma once
#include "NxCore/Object.h"	
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

	class NEXUS_GRAPHICS_API Texture : public BaseAsset
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		std::string GetAssetTypeString() override { return "TextureAsset"; }
		uint8_t GetAssetTypeIndex() override { return 3; }

		virtual void PrepareForRender() {};
	};

	struct SamplerSpecification
	{
		Meshing::Sampler sampler;
	};

	class NEXUS_GRAPHICS_API Sampler
	{
	public:
		Sampler() = default;
		virtual ~Sampler() = default;
	};
}