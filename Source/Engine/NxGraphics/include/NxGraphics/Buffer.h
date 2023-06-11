#pragma once
#include "TypeImpls.h"

namespace Nexus
{
	class StaticBuffer
	{
	public:
		StaticBuffer() = default;
		virtual ~StaticBuffer() = default;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void Update(void* data) = 0;
	};
}