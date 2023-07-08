#include "NxVulkan/VkBuffer.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkCommandQueue.h"

static VkBufferUsageFlagBits GetVulkanBufferType(Nexus::BufferType Type)
{
	switch (Type)
	{
		case Nexus::BufferType::Vertex: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case Nexus::BufferType::Index: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		case Nexus::BufferType::Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		case Nexus::BufferType::Storage: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		default:
			NEXUS_ASSERT(1, "Invalid Buffer Type");
			return (VkBufferUsageFlagBits)0;
	}
}

static const char* GetVulkanBufferTypeName(Nexus::BufferType Type)
{
	switch (Type)
	{
		case Nexus::BufferType::Vertex: return "Vertex";
		case Nexus::BufferType::Index: return "Index";
		case Nexus::BufferType::Uniform: return "Uniform";
		case Nexus::BufferType::Storage: return "Storage";
		default: return "";
	}
}

Nexus::VulkanBuffer::VulkanBuffer(const BufferSpecification& specs)
{
	m_Type = specs.type;
	m_Size = specs.size;
	m_stagAlloc = nullptr;
	m_stagBuf = nullptr;

	VmaAllocationCreateInfo aInfo{};
	aInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VkBufferCreateInfo bInfo{};
	bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bInfo.pNext = nullptr;
	bInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bInfo.size = specs.size;
	bInfo.usage = GetVulkanBufferType(specs.type);
	
	if (specs.cpuMemory)
		aInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
	else
		bInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	auto allocator = VulkanContext::Get()->GetDeviceRef()->GetAllocator();
	_VKR = vmaCreateBuffer(allocator, &bInfo, &aInfo, &m_Buffer, &m_Alloc, &m_Info);

	if (specs.cpuMemory && specs.data)
	{
		if (m_Info.pMappedData)
			memcpy(m_Info.pMappedData, specs.data, m_Size);

	}
		
	CHECK_HANDLE(m_Buffer, VkBuffer);
	NEXUS_LOG("Vulkan", "%s Buffer Created : size-%i", GetBufferTypeStringName(m_Type).c_str(), m_Size);
	
	if (!specs.cpuMemory && specs.data)
	{
		if (specs.type == BufferType::Vertex || specs.type == BufferType::Index)
		{
			bInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			aInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

			VmaAllocationInfo allocInfo{};
			vmaCreateBuffer(allocator, &bInfo, &aInfo, &m_stagBuf, &m_stagAlloc, &allocInfo);

			if (allocInfo.pMappedData)
				memcpy(allocInfo.pMappedData, specs.data, m_Size);

			VulkanCommandQueue::Get()->TransferBufferToGPU(this);;
		}
	}
}

Nexus::VulkanBuffer::~VulkanBuffer()
{
	auto allocator = VulkanContext::Get()->GetDeviceRef()->GetAllocator();
	
	if (m_stagBuf)
		vmaDestroyBuffer(allocator, m_stagBuf, m_stagAlloc);
	
	vmaDestroyBuffer(allocator, m_Buffer, m_Alloc);
	NEXUS_LOG("Vulkan", "%s Buffer Destroyed : size-%i", GetBufferTypeStringName(m_Type).c_str(), m_Size);
}

void Nexus::VulkanBuffer::Update(void* data)
{
	if (!data) return;
	if (!m_Info.pMappedData) return;

	memcpy(m_Info.pMappedData, data, m_Size);
}
