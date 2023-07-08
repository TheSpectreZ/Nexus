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
	class NEXUS_VULKAN_API VulkanBuffer : public Buffer
	{
		friend class VulkanCommandQueue;
	public:
		VulkanBuffer(const BufferSpecification& specs);
		~VulkanBuffer() override;

		VkBuffer Get() { return m_Buffer; }
		uint32_t GetSize() override { return m_Size; }
		void Update(void* data) override;
	private:
		BufferType m_Type; uint32_t m_Size;
		VkBuffer m_Buffer, m_stagBuf;
		VmaAllocation m_Alloc, m_stagAlloc;
		VmaAllocationInfo m_Info;
	};
}