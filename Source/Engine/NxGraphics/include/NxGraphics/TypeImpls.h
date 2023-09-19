#pragma once
#include <string>
#include "NxCore/Base.h"

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif // NEXUS_GRAPHICS_SHARED_BUILD

namespace Nexus
{
	struct NEXUS_GRAPHICS_API Viewport
	{
		float x, y, width, height, minDepth, maxDepth;
	};

	struct NEXUS_GRAPHICS_API Offset
	{
		int x, y;
	};

	struct NEXUS_GRAPHICS_API Extent
	{
		unsigned int width, height;
	};

	struct NEXUS_GRAPHICS_API Scissor
	{
		Offset Offset;
		Extent Extent;
	};

	enum class NEXUS_GRAPHICS_API ShaderStage
	{
		None = 0, Vertex = 1, Fragment = 2, Compute = 4
	};
	ENUM_FLAG_OPERATORS(ShaderStage)
	
	enum class NEXUS_GRAPHICS_API BufferType
	{
		Vertex, Index, Uniform, Storage
	};

	enum class NEXUS_GRAPHICS_API TextureType
	{
		TwoDim, ThreeDim, Cube
	};

	enum class NEXUS_GRAPHICS_API TextureUsage
	{
		ColorAttachment,
		DepthStencilAttachment,

		ShaderSampled,
		StorageWrite,
		StorageRead
	};

	enum class NEXUS_GRAPHICS_API TextureFormat
	{
		SWAPCHAIN_COLOR,
		SWAPCHAIN_DEPTH,

		RGBA8_SRGB,
		
		RG16_SFLOAT,
		RG32_SFLOAT,

		RGBA16_SFLOAT,
		RGBA32_SFLOAT,
	};

	enum class NEXUS_GRAPHICS_API ShaderResourceType
	{
		None, Uniform, SampledImage, StorageImage, SeperateImage
	};

	enum class NEXUS_GRAPHICS_API SamplerFilter
	{
		Nearest = 1, Linear = 2
	};

	enum class NEXUS_GRAPHICS_API SamplerWrapMode
	{
		Repeat = 1, Mirrored_Repeat = 2, Clamped_To_Edge = 3
	};

	std::string NEXUS_GRAPHICS_API GetShaderStageTypeStringName(ShaderStage stage);

	std::string NEXUS_GRAPHICS_API GetBufferTypeStringName(BufferType Type);

	std::string NEXUS_GRAPHICS_API GetShaderResourceTypeStringName(ShaderResourceType Type);
}