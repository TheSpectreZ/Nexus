#include "Backend.h"
#include "vkAssert.h"

#include "Graphics/Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void TransitionLayout(VkCommandBuffer cmd, VkImageLayout src, VkImageLayout dst, VkImage image,uint32_t layers)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = src;
	barrier.newLayout = dst;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage, dstStage;

	if (src == VK_IMAGE_LAYOUT_UNDEFINED && dst == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (src == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && dst == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		NEXUS_ASSERT(1, "Unsupported Image Transition");
	}

	for (uint32_t i = 0; i < layers; i++)
	{
		barrier.subresourceRange.baseArrayLayer = i;
		vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

}

void Copy(VkCommandBuffer cmd, VkBuffer src, VkImage image, VkExtent2D extent, uint32_t layers)
{
	TransitionLayout(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image,layers);

	std::vector<VkBufferImageCopy> copy(layers);
	
	for (uint32_t i = 0; i < layers; i++)
	{
		copy[i].bufferOffset = 0;
		copy[i].bufferRowLength = 0;
		copy[i].bufferImageHeight = 0;

		copy[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy[i].imageSubresource.mipLevel = 0;
		copy[i].imageSubresource.baseArrayLayer = i;
		copy[i].imageSubresource.layerCount = 1;

		copy[i].imageOffset = { 0, 0, 0 };
		copy[i].imageExtent = { extent.width,extent.height,1 };
	}

	vkCmdCopyBufferToImage(cmd, src, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layers, copy.data());

	TransitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image,layers);
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

void Nexus::Graphics::Texture2D::Create(const char* filepath)
{
	int w, h, c;
	unsigned char* pixels = stbi_load(filepath, &w, &h, &c, 4);

	Create(pixels, VK_SAMPLE_COUNT_1_BIT, { (uint32_t)w,(uint32_t)h }, VK_FORMAT_R8G8B8A8_UNORM);

	stbi_image_free(pixels);
}

void Nexus::Graphics::Texture2D::Create(void* pixelData,VkSampleCountFlagBits samples, VkExtent2D extent,VkFormat format)
{
	m_extent = extent;
	VkDeviceSize size = (VkDeviceSize)extent.width * extent.height * 4;

	// Staging
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size = size;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo info{};
		info.usage = VMA_MEMORY_USAGE_AUTO;
		info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		
		VmaAllocationInfo ainfo{};
		vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &stagingBuffer, &stagingAllocation, &ainfo);

		memcpy(ainfo.pMappedData, pixelData, size);
	}

	VkImageCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	Info.pNext = nullptr;
	Info.arrayLayers = 1;
	Info.extent = { extent.width,extent.height,1 };
	Info.imageType = VK_IMAGE_TYPE_2D;
	Info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Info.mipLevels = 1;
	Info.samples = samples;
	Info.flags = 0;
	Info.tiling = VK_IMAGE_TILING_OPTIMAL;
	Info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	Info.format = format;
	Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo info{};
	info.usage = VMA_MEMORY_USAGE_AUTO;

	VmaAllocationInfo ainfo{};
	_VKR = vmaCreateImage(Backend::GetAllocator(), &Info, &info, &m_Image, &m_allocation, &ainfo);
	CHECK_HANDLE(m_Image,VkImage)
	NEXUS_LOG_TRACE("Texture2D Created: {0}x{1}",extent.width,extent.height)

	VkCommandBuffer cmd = Backend::BeginSingleTimeCommands();
	Copy(cmd, stagingBuffer, m_Image, extent, 1);
	Backend::EndSingleTimeCommands(cmd);

	vmaDestroyBuffer(Backend::GetAllocator(), stagingBuffer, stagingAllocation);

	// Image View
	{
		VkImageViewCreateInfo i{};
		i.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		i.pNext = nullptr;
		i.image = m_Image;
		i.viewType = VK_IMAGE_VIEW_TYPE_2D;
		i.format = format;
		i.flags = 0;
		i.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		i.subresourceRange.baseMipLevel = 0;
		i.subresourceRange.levelCount = 1;
		i.subresourceRange.baseArrayLayer = 0;
		i.subresourceRange.layerCount = 1;

		_VKR = vkCreateImageView(Backend::GetDevice(), &i, nullptr, &m_view);
		CHECK_LOG_VKR
	}
}

void Nexus::Graphics::Texture2D::Destroy()
{
	vmaDestroyImage(Backend::GetAllocator(), m_Image, m_allocation);
	vkDestroyImageView(Backend::GetDevice(), m_view, nullptr);
}

void Nexus::Graphics::Sampler::Create(VkFilter Mag, VkFilter Min, VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w)
{
	VkSamplerCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	Info.pNext = nullptr;

	Info.magFilter = Mag;
	Info.minFilter = Min;
	Info.addressModeU = u;
	Info.addressModeV = v;
	Info.addressModeW = w;
	Info.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties Props;
	vkGetPhysicalDeviceProperties(Backend::GetPhysicalDevice(), & Props);

	Info.maxAnisotropy = Props.limits.maxSamplerAnisotropy;
	Info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	Info.unnormalizedCoordinates = VK_FALSE;
	Info.compareEnable = VK_FALSE;
	Info.compareOp = VK_COMPARE_OP_ALWAYS;
	Info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	Info.mipLodBias = 0.f;
	Info.minLod = 0.f;
	Info.maxLod = 1;

	vkCreateSampler(Backend::GetDevice(), &Info, nullptr, &m_sampler);
}

void Nexus::Graphics::Sampler::Destroy()
{
	vkDestroySampler(Backend::GetDevice(), m_sampler, nullptr);
}

void Nexus::Graphics::CubeMapTexture::Create(std::array<const char*, 6> filepath)
{
	std::vector<unsigned char*> pixels(6);

	int w, h, c;
	for (uint32_t i = 0; i < 6; i++)
	{
		pixels[i] = stbi_load(filepath[i], &w, &h, &c, 4);
	}

	VkDeviceSize ImageSize = w * h * 4;

	// Staging
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	{
		VkBufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.size =ImageSize * 6;
		Info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo info{};
		info.usage = VMA_MEMORY_USAGE_AUTO;
		info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(Backend::GetAllocator(), &Info, &info, &stagingBuffer, &stagingAllocation, nullptr);

		void* data;
		vmaMapMemory(Backend::GetAllocator(), stagingAllocation, &data);
		for (uint32_t i = 0; i < 6; i++)
		{
			memcpy(data, pixels[i], ImageSize);
			data = (void*)((uint8_t*)data + ImageSize);
		} 
		vmaUnmapMemory(Backend::GetAllocator(), stagingAllocation);
		
	}

	for (auto& p : pixels)
		stbi_image_free(p);

	// Image
	{
		VkImageCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.imageType = VK_IMAGE_TYPE_2D;
		Info.format = VK_FORMAT_R8G8B8A8_UNORM;
		Info.extent.width = w;
		Info.extent.height = h;
		Info.extent.depth = 1;
		Info.mipLevels = 1;
		Info.arrayLayers = 6;
		Info.samples = VK_SAMPLE_COUNT_1_BIT;
		Info.tiling = VK_IMAGE_TILING_OPTIMAL;
		Info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		Info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		VmaAllocationCreateInfo a{};
		a.usage = VMA_MEMORY_USAGE_AUTO;

		vmaCreateImage(Backend::GetAllocator(), &Info, &a, &m_Image, &m_Allocation, nullptr);
		CHECK_HANDLE(m_Image, VkImage)
		NEXUS_LOG_TRACE("CubeMap Texture Created: {0}x{1}x6", w, h)
	}

	VkCommandBuffer cmd = Backend::BeginSingleTimeCommands();
	Copy(cmd, stagingBuffer, m_Image, VkExtent2D(w, h), 6);
	Backend::EndSingleTimeCommands(cmd);

	vmaDestroyBuffer(Backend::GetAllocator(), stagingBuffer, stagingAllocation);

	// Views
	{
		VkImageViewCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		Info.pNext = nullptr;
		Info.image = m_Image;
		Info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		Info.format = VK_FORMAT_R8G8B8A8_UNORM;
		Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Info.subresourceRange.baseMipLevel = 0;
		Info.subresourceRange.levelCount = 1;
		Info.subresourceRange.baseArrayLayer = 0;
		Info.subresourceRange.layerCount = 6;

		vkCreateImageView(Backend::GetDevice(), &Info, nullptr, &m_View);
		CHECK_HANDLE(m_View, VkImageView)
	}
}

void Nexus::Graphics::CubeMapTexture::Destroy()
{
	vmaDestroyImage(Backend::GetAllocator(), m_Image, m_Allocation);
	vkDestroyImageView(Backend::GetDevice(), m_View, nullptr);
}

void Nexus::Graphics::RenderCommand::DrawIndexed(VkCommandBuffer cmd, uint32_t IndexCount)
{
	vkCmdDrawIndexed(cmd, IndexCount, 1, 0, 0, 0);
}
