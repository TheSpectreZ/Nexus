#pragma once

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
		Vertex, Fragment
	};
	
	enum class BufferType
	{
		Vertex, Index
	};

	enum class ShaderResourceType
	{
		Uniform, SampledImage
	};

	std::string GetShaderStageTypeStringName(ShaderStage stage);

	std::string GetBufferTypeStringName(BufferType Type);
	
	std::string GetShaderResourceTypeStringName(ShaderResourceType Type);
	
}