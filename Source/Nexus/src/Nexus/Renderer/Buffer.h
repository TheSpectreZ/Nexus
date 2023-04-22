#pragma once
#include "RenderTypes.h"

namespace Nexus
{
	class StaticBuffer
	{
	public:
		static Ref<StaticBuffer> Create(uint32_t size, BufferType Type, void* data);

		StaticBuffer() = default;
		virtual ~StaticBuffer() = default;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(uint32_t size);

		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void Update(void* data) = 0;
	};

}
