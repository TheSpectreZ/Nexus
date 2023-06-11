#pragma once
#include "NxGraphics/Buffer.h"
#include "VkAssert.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanStaticBuffer : public StaticBuffer
	{
		friend class VulkanCommandQueue;
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

	class NEXUS_VULKAN_API VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size);
		~VulkanUniformBuffer();

		void Update(void* data) override;

		VkBuffer Get() { return m_buffer; }
		uint32_t GetSize() { return (uint32_t)m_allocInfo.size; }
	private:
		VkBuffer m_buffer;
		VmaAllocationInfo m_allocInfo;
		VmaAllocation m_allocation;
	};

}