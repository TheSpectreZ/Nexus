#pragma once
#include <string>

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif // NEXUS_GRAPHICS_SHARED_BUILD

namespace Nexus
{
	struct Viewport
	{
		float x, y, width, height, minDepth, maxDepth;
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

	enum class ShaderStage
	{
		Vertex = 0, Fragment = 1
	};
	
	enum class BufferType
	{
		Vertex, Index
	};

	enum class ShaderResourceType
	{
		Uniform, SampledImage
	};

	enum class SamplerFilter
	{
		Nearest, Linear
	};

	enum class SamplerWrapMode
	{
		Repeat, Mirrored_Repeat, Clamped_To_Edge
	};

	std::string NEXUS_GRAPHICS_API GetShaderStageTypeStringName(ShaderStage stage);

	std::string NEXUS_GRAPHICS_API GetBufferTypeStringName(BufferType Type);

	std::string NEXUS_GRAPHICS_API GetShaderResourceTypeStringName(ShaderResourceType Type);
}