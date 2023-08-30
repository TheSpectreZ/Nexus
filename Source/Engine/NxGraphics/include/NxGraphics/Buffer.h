#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	struct BufferSpecification
	{
		std::string debugName;
		BufferType type;
		uint32_t size;
		uint32_t stride;
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