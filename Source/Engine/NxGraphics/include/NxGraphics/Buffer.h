#pragma once
#include "Specification.h"

namespace Nexus
{
	struct GPUBufferSpecification
	{
		GPUBufferType type;
		uint32_t size;
	};

	class GPUBuffer
	{
	public:
		virtual ~GPUBuffer() = default;
	};
}