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

	VulkanCommandQueue::Get()->TransferBufferToGPU(this);

	NEXUS_LOG("Vulkan", "Static %s Buffer Created | size: %i", GetVulkanBufferTypeName(m_Type), m_size);
}

Nexus::VulkanStaticBuffer::~VulkanStaticBuffer()
{
	vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_buffer, m_allocation);
	
	if (m_StagingBuff != nullptr)
		vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_StagingBuff, m_StagingAlloc);

	NEXUS_LOG("Vulkan", "Static %s Buffer Destroyed", GetVulkanBufferTypeName(m_Type));
}

#pragma endregion

#pragma region UniformBuffer

Nexus::VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
{
	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Info.size = size;
	Info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo aInfo{};
	aInfo.usage = VMA_MEMORY_USAGE_AUTO;
	aInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	_VKR = vmaCreateBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), &Info, &aInfo, &m_buffer, &m_allocation, &m_allocInfo);
	CHECK_HANDLE(m_buffer, VkBuffer);
	NEXUS_LOG("Vulkan", "Uniform Buffer Created");
}

Nexus::VulkanUniformBuffer::~VulkanUniformBuffer()
{
	vmaDestroyBuffer(VulkanContext::Get()->GetDeviceRef()->GetAllocator(), m_buffer, m_allocation);
	NEXUS_LOG("Vulkan", "Uniform Buffer Destroyed");
}

void Nexus::VulkanUniformBuffer::Update(void* data)
{
	memcpy(m_allocInfo.pMappedData, data, m_allocInfo.size);
}

#pragma endregion

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
	VmaAllocationInfo alInfo{};
	_VKR = vmaCreateBuffer(allocator, &bInfo, &aInfo, &m_Buffer, &m_Alloc, &alInfo);

	if (specs.cpuMemory && specs.data)
	{
		if (alInfo.pMappedData)
			memcpy(alInfo.pMappedData, specs.data, m_Size);

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
