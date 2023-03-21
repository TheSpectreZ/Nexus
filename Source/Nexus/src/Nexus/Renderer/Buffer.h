#pragma once

namespace Nexus
{
	enum class BufferType
	{
		VERTEX, INDEX
	};

	class StaticBuffer
	{
	public:
		static Ref<StaticBuffer> Create(uint32_t size, BufferType Type, void* data);

		StaticBuffer() = default;
		virtual ~StaticBuffer() = default;
	};

	class DynamicBuffer
	{
	public:
		static Ref<DynamicBuffer> Create(uint32_t size, BufferType Type);

		DynamicBuffer() = default;
		virtual ~DynamicBuffer() = default;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(uint32_t size);

		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;
	};

}
