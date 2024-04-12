#pragma once
#include <cstdint>

namespace Nexus
{
	struct Viewport
	{
		float x, y;
		float width, height;
		float minDepth, maxDepth;
	};

	struct Offset
	{
		int x, y;
	};

	struct Extent
	{
		unsigned int width, height;
	};

	struct Scissor
	{
		Offset Offset;
		Extent Extent;
	};

	enum class ShaderType
	{
		None,
		Vertex,
		Fragment,
		Compute
	};

	enum class BufferType
	{
		Vertex,
		Index,
		Uniform,
		Storage
	};

	enum class TextureType
	{
		TwoD, ThreeD, Cube
	};

	enum class TextureFormat
	{
		RGBA8_SRGB,

		RG16_SFLOAT,
		RG32_SFLOAT,

		RGBA16_SFLOAT,
		RGBA32_SFLOAT
	};

	enum class ShaderResourceType
	{
		None,
		Uniform,
		SampledImage,
		StorageImage,
		SeperateImage
	};

	struct BufferSpecifications
	{
		BufferType type;
		uint32_t size;
		bool bCpuMemory;
		void* data;
	};

	struct ImageSpecifications
	{

	};
}

#define ENUM_TO_STRING(EnumValue) #EnumValue