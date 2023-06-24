#pragma once
#include <string>

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
		Vertex = 0, Fragment = 1
	};
	
	enum class NEXUS_GRAPHICS_API BufferType
	{
		Vertex, Index, Uniform, Storage
	};

	enum class NEXUS_GRAPHICS_API ShaderResourceType
	{
		Uniform, SampledImage
	};

	enum class NEXUS_GRAPHICS_API SamplerFilter
	{
		Nearest, Linear
	};

	enum class NEXUS_GRAPHICS_API SamplerWrapMode
	{
		Repeat, Mirrored_Repeat, Clamped_To_Edge
	};

	std::string NEXUS_GRAPHICS_API GetShaderStageTypeStringName(ShaderStage stage);

	std::string NEXUS_GRAPHICS_API GetBufferTypeStringName(BufferType Type);

	std::string NEXUS_GRAPHICS_API GetShaderResourceTypeStringName(ShaderResourceType Type);
}