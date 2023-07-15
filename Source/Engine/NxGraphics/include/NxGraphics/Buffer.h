#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	struct BufferSpecification
	{
		BufferType type;
		uint32_t size;
		bool cpuMemory;
		void* data;
	};

	class Buffer
	{
	public:
		Buffer() = default;
		virtual ~Buffer() = default;

		virtual uint32_t GetSize() = 0;
		virtual void Update(void* data) = 0;
	};
}