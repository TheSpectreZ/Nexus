#include "nxpch.h"
#include "VkTexture.h"
#include "VkTransferCommandQueue.h"
#include "VkContext.h"
#include "VkSwapchain.h"

Nexus::VulkanTexture::VulkanTexture(const TextureCreateInfo& info)
	:m_Extent(info.extent)
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	Ref<VulkanSwapchain> swapchain = VulkanSwapchain::Get();

	// Image
	{
		VkImageCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.extent = { info.extent.width,info.extent.height,1 };
		Info.arrayLayers = 1;
		Info.flags = 0;
		Info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		Info.mipLevels = 1;
		Info.pQueueFamilyIndices = nullptr;
		Info.queueFamilyIndexCount = 0;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.tiling = VK_IMAGE_TILING_OPTIMAL;
		Info.imageType = VK_IMAGE_TYPE_2D;
		Info.samples = VK_SAMPLE_COUNT_1_BIT;
		Info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		Info.format = VK_FORMAT_R8G8B8A8_SRGB; 

		VmaAllocationCreateInfo aInfo{};
		aInfo.usage = VMA_MEMORY_USAGE_AUTO;

		_VKR = vmaCreateImage(device->GetAllocator(), &Info, &aInfo, &m_Image, &m_Alloc, nullptr);
		CHECK_HANDLE(m_Image, VkImage);
	}

	// View
	{
		VkImageViewCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		Info.image = m_Image;
		Info.components.r = VK_COMPONENT_SWIZZLE_R;
		Info.components.g = VK_COMPONENT_SWIZZLE_G;
		Info.components.b = VK_COMPONENT_SWIZZLE_B;
		Info.components.a = VK_COMPONENT_SWIZZLE_A;
		Info.subresourceRange.layerCount = 1;
		Info.subresourceRange.levelCount = 1;
		Info.subresourceRange.baseArrayLayer = 0;
		Info.subresourceRange.baseMipLevel = 0;
		Info.format = VK_FORMAT_R8G8B8A8_SRGB;
		Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		_VKR = vkCreateImageView(device->Get(), &Info, nullptr, &m_View);
		CHECK_HANDLE(m_View, VkImageView);
	}

	// Staging 
	{
		VkDeviceSize size = (VkDeviceSize)info.extent.width * info.extent.height * 4;

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
		vmaCreateBuffer(device->GetAllocator(), &Info, &aInfo, &m_StagingBuffer, &m_StagingAlloc, &allocInfo);

		if (allocInfo.pMappedData)
		{
			memcpy(allocInfo.pMappedData, info.pixeldata, size);
		}
	}
}

Nexus::VulkanTexture::~VulkanTexture()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	if (m_StagingBuffer != nullptr)
		vmaDestroyBuffer(device->GetAllocator(), m_StagingBuffer, m_StagingAlloc);

	vmaDestroyImage(device->GetAllocator(), m_Image, m_Alloc);
	vkDestroyImageView(device->Get(), m_View, nullptr);
}
