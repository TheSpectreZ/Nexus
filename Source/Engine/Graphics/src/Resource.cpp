#include "Backend.h"
#include "Graphics/Resource.h"
#include "vkAssert.h"
#include <unordered_map>

void Copy(VkCommandBuffer cmd, VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkBufferCopy copy{};
	copy.size = size;
	copy.dstOffset = 0;
	copy.srcOffset = 0;

	vkCmdCopyBuffer(cmd, src, dst, 1, &copy);
	NEXUS_LOG_DEBUG("Buffer Copied: size-{0}", size);
}

void Nexus::Graphics::VertexBuffer::Create(uint32_t count, VkDeviceSize stride,void* data)
{
	// Staging
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size = stride * count;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo info{};
		info.usage = VMA_MEMORY_USAGE_AUTO;
		info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo ainfo{};
		vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &stagingBuffer, &stagingAllocation, &ainfo);

		memcpy(ainfo.pMappedData, data, stride * count);
	}

	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Info.size = stride * count;
	Info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo info{};
	info.usage = VMA_MEMORY_USAGE_AUTO;

	_VKR = vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &m_buffer, &m_allocation, nullptr);
	CHECK_HANDLE(m_buffer, VkBuffer)
	NEXUS_LOG_TRACE("Vertex Buffer Created: {0}", count * stride)

	auto cmdbuffer = Backend::BeginSingleTimeCommands();
	Copy(cmdbuffer, stagingBuffer, m_buffer, stride * count);
	Backend::EndSingleTimeCommands(cmdbuffer);

	vmaDestroyBuffer(Backend::GetAllocator(), stagingBuffer, stagingAllocation);
}

void Nexus::Graphics::VertexBuffer::Destroy()
{
	vmaDestroyBuffer(Backend::GetAllocator(), m_buffer, m_allocation);
}

void Nexus::Graphics::VertexBuffer::Bind(VkCommandBuffer buffer)
{
	VkDeviceSize off[] = { 0 };
	vkCmdBindVertexBuffers(buffer, 0, 1, &m_buffer, off);
}

void Nexus::Graphics::IndexBuffer::Create(uint32_t count,VkDeviceSize stride,void* data)
{
	m_indices = count;

	// Staging
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size = stride * count;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo info{};
		info.usage = VMA_MEMORY_USAGE_AUTO;
		info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo ainfo{};
		vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &stagingBuffer, &stagingAllocation, &ainfo);

		memcpy(ainfo.pMappedData, data, stride * count);
	}

	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Info.size = stride * count;
	Info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo info{};
	info.usage = VMA_MEMORY_USAGE_AUTO;

	_VKR = vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &m_buffer, &m_allocation, nullptr);
	CHECK_HANDLE(m_buffer, VkBuffer)
	NEXUS_LOG_TRACE("Index Buffer Created: {0} , {1}", m_indices, m_indices * stride)

	auto cmdbuffer = Backend::BeginSingleTimeCommands();
	Copy(cmdbuffer, stagingBuffer, m_buffer, stride * count);
	Backend::EndSingleTimeCommands(cmdbuffer);

	vmaDestroyBuffer(Backend::GetAllocator(), stagingBuffer, stagingAllocation);
}

void Nexus::Graphics::IndexBuffer::Destroy()
{
	vmaDestroyBuffer(Backend::GetAllocator(), m_buffer, m_allocation);
}

void Nexus::Graphics::IndexBuffer::Bind(VkCommandBuffer buffer)
{
	vkCmdBindIndexBuffer(buffer, m_buffer, 0, VK_INDEX_TYPE_UINT32);
}

void Nexus::Graphics::UniformBuffer::Create(VkDeviceSize size)
{
	m_size = size;

	VkBufferCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Info.pNext = nullptr;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	Info.size = size;
	Info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo info{};
	info.usage = VMA_MEMORY_USAGE_AUTO;
	info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	_VKR = vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &m_buffer,&m_allocation, nullptr);
	CHECK_HANDLE(m_buffer,VkBuffer)
	NEXUS_LOG_TRACE("Uniform Buffer Created: {0}",size);

	vmaMapMemory(Backend::GetAllocator(), m_allocation, &m_Data);
}

void Nexus::Graphics::UniformBuffer::Destroy()
{
	vmaUnmapMemory(Backend::GetAllocator(), m_allocation);
	vmaDestroyBuffer(Backend::GetAllocator(), m_buffer, m_allocation);
}

void Nexus::Graphics::UniformBuffer::Update(void* data)
{
	memcpy(m_Data, data, m_size);
}
