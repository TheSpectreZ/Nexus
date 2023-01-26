#include "Graphics/Resource.h"
#include "Backend.h"
#include "vkAssert.h"

void Nexus::Graphics::Buffer::Create(VkBufferUsageFlagBits usage, VkDeviceSize size)
{
	m_size = size;

	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.usage = usage;
	Info.size = size;
	Info.flags = 0;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	_VKR = vkCreateBuffer(Backend::GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkBuffer)
	NEXUS_LOG_TRACE("Vulkan Buffer Created")
}

void Nexus::Graphics::Buffer::Destroy()
{
	vkDestroyBuffer(Backend::GetDevice(), m_handle, nullptr);
}

void Nexus::Graphics::Buffer::CopyBuffer(VkCommandBuffer buffer,Buffer& src, Buffer& dst, VkDeviceSize size)
{
	VkBufferCopy copy{};

	copy.dstOffset = 0;
	copy.srcOffset = 0;
	copy.size = size;

	vkCmdCopyBuffer(buffer, src.Get(), dst.Get(), 1, &copy);
}

void Nexus::Graphics::Memory::Allocate(Buffer& buffer, VkMemoryPropertyFlags flags)
{
	m_size = buffer.GetSize();

	VkMemoryRequirements req{};
	vkGetBufferMemoryRequirements(Backend::GetDevice(), buffer.Get(), &req);

	VkMemoryAllocateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.allocationSize = req.size;
	Info.memoryTypeIndex = FindMemoryType(Backend::GetPhysicalDevice(), req.memoryTypeBits, flags);

	_VKR = vkAllocateMemory(Backend::GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle,VkDeviceMemory);

	_VKR = vkBindBufferMemory(Backend::GetDevice(), buffer.Get(), m_handle, 0);
	CHECK_LOG_VKR

	NEXUS_LOG_TRACE("Vulkan Memory Allocated: {0}",req.size)
}

void Nexus::Graphics::Memory::Free()
{
	vkFreeMemory(Backend::GetDevice(), m_handle, nullptr);
}

void Nexus::Graphics::Memory::Map()
{
	vkMapMemory(Backend::GetDevice(), m_handle, 0, m_size, 0, &m_data);
}

void Nexus::Graphics::Memory::Update(void* data)
{
	memcpy(m_data, data, m_size);
}

void Nexus::Graphics::Memory::UnMap()
{
	vkUnmapMemory(Backend::GetDevice(), m_handle);
}
