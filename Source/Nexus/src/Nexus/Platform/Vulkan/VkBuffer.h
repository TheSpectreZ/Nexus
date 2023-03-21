#pragma once
#include "Renderer/Buffer.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanCommand;
	class VulkanTransferCommandQueue;
	
	class VulkanStaticBuffer : public StaticBuffer
	{
		friend class VulkanCommand;
		friend class VulkanTransferCommandQueue;
	public:
		VulkanStaticBuffer(uint32_t size, BufferType Type, void* data);
		~VulkanStaticBuffer() override;

		VkBuffer Get() { return m_buffer; }
	private:
		uint32_t m_size;
		BufferType m_Type;
		VkBuffer m_buffer, m_StagingBuff;
		VmaAllocation m_allocation, m_StagingAlloc;
	};

	class VulkanDynamicBuffer : public DynamicBuffer
	{
	public:
		VulkanDynamicBuffer(uint32_t size, BufferType Type);
	};

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size);
		~VulkanUniformBuffer();
	private:
		uint32_t m_size;
		VkBuffer m_buffer;
		VmaAllocation m_allocation;
	};

}