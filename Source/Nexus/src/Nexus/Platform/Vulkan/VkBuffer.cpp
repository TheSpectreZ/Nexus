#include "nxpch.h"
#include "VkBuffer.h"
#include "VkContext.h"
#include "VkTransferCommandQueue.h"
#include "Renderer/Renderer.h"

static VkBufferUsageFlagBits GetVulkanBufferType(Nexus::BufferType Type)
{
	switch (Type)
	{
		case Nexus::BufferType::VERTEX: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case Nexus::BufferType::INDEX: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		default:
			NEXUS_ASSERT(1, "Invalid Buffer Type");
			return (VkBufferUsageFlagBits)0;
	}
}

static const char* GetVulkanBufferTypeName(Nexus::BufferType Type)
{
	switch (Type)
	{
		case Nexus::BufferType::VERTEX: return "Vertex";
		case Nexus::BufferType::INDEX: return "Index";
		default: return "";
	}
}

#pragma region StaticBuffer

Nexus::VulkanStaticBuffer::VulkanStaticBuffer(uint32_t size, BufferType Type, void* data)
	:m_Type(Type),m_size(size)
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size = size;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | GetVulkanBufferType(Type);
	
		VmaAllocationCreateInfo aInfo{};
		aInfo.usage = VMA_MEMORY_USAGE_AUTO;
	
		vmaCreateBuffer(device->GetAllocator(), &Info, &aInfo, &m_buffer, &m_allocation, nullptr);
	}

	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size = size;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo aInfo{};
		aInfo.usage = VMA_MEMORY_USAGE_AUTO;
		aInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo allocInfo{};
		vmaCreateBuffer(device->GetAllocator(), &Info, &aInfo, &m_StagingBuff, &m_StagingAlloc, &allocInfo);
		
		if(allocInfo.pMappedData)
		{
			memcpy(allocInfo.pMappedData, data, size);
		}
		
	}

	NEXUS_LOG_TRACE("Vulkan Static {0} Buffer Created | size: {1}", GetVulkanBufferTypeName(m_Type), m_size);
}

Nexus::VulkanStaticBuffer::~VulkanStaticBuffer()
{
	vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_buffer, m_allocation);
	
	if (m_StagingBuff != nullptr)
		vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_StagingBuff, m_StagingAlloc);

	NEXUS_LOG_TRACE("Vulkan Static {0} Buffer Destroyed", GetVulkanBufferTypeName(m_Type));
}

#pragma endregion

#pragma region DynamicBuffer

Nexus::VulkanDynamicBuffer::VulkanDynamicBuffer(uint32_t size, BufferType Type)
{
}

#pragma endregion

#pragma region UniformBuffer

Nexus::VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
	:m_size(size)
{
	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Info.size = m_size;
	Info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo aInfo{};
	aInfo.usage = VMA_MEMORY_USAGE_AUTO;
	
	_VKR = vmaCreateBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), &Info, &aInfo, &m_buffer, &m_allocation, nullptr);
	CHECK_HANDLE(m_buffer, VkBuffer);
}

Nexus::VulkanUniformBuffer::~VulkanUniformBuffer()
{
	vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_buffer, m_allocation);
}

#pragma endregion