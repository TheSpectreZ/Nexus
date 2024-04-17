#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	struct ShaderResouceHeapLayoutBinding
	{
		uint32_t bindPoint, bufferSize, arrayCount;
		ShaderResourceType type;
		ShaderStage stage;
	};

	class ShaderResourceHeapLayout
	{
	public:
		ShaderResourceHeapLayout() = default;
		virtual ~ShaderResourceHeapLayout() = default;
	};

	class ShaderResourceHeap
	{
	public:
		ShaderResourceHeap() = default;
		virtual ~ShaderResourceHeap() = default;
	};

	class ShaderResourcePool
	{
	public:
		ShaderResourcePool() = default;
		virtual ~ShaderResourcePool() = default;
	};
}