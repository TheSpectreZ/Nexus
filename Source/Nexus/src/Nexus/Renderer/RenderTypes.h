#pragma once

namespace Nexus
{
	typedef uint32_t ResourceID;

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
	ENUM_FLAG_OPERATORS(ShaderStage)
	
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


	std::string GetShaderStageTypeStringName(ShaderStage stage);

	std::string GetBufferTypeStringName(BufferType Type);
	
	std::string GetShaderResourceTypeStringName(ShaderResourceType Type);
	
}