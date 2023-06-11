#include "NxVulkan/VkTexture.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkCommandQueue.h"

Nexus::VulkanTexture::VulkanTexture(const TextureCreateInfo& info)
	:m_Extent(info.extent)
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	VulkanSwapchain* swapchain = VulkanSwapchain::Get();

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

	VulkanCommandQueue::Get()->TransferTextureToGPU(this);

	NEXUS_LOG("Vulkan", "Texture Created");
}

Nexus::VulkanTexture::~VulkanTexture()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	if (m_StagingBuffer != nullptr)
		vmaDestroyBuffer(device->GetAllocator(), m_StagingBuffer, m_StagingAlloc);

	vmaDestroyImage(device->GetAllocator(), m_Image, m_Alloc);
	vkDestroyImageView(device->Get(), m_View, nullptr);

	NEXUS_LOG("Vulkan", "Texture Destroyed");
}

VkFilter GetVulkanSamplerFilter(Nexus::SamplerFilter filter)
{
	switch (filter)
	{
	case Nexus::SamplerFilter::Nearest:
		return VK_FILTER_NEAREST;
	case Nexus::SamplerFilter::Linear:
		return VK_FILTER_LINEAR;
	default:
		return VK_FILTER_MAX_ENUM;
	}
}

VkSamplerAddressMode GetVulkanSamplerWrapMode(Nexus::SamplerWrapMode mode)
{
	switch (mode)
	{
	case Nexus::SamplerWrapMode::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case Nexus::SamplerWrapMode::Mirrored_Repeat:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case Nexus::SamplerWrapMode::Clamped_To_Edge:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	default:
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

Nexus::VulkanSampler::VulkanSampler(SamplerFilter Near, SamplerFilter Far, SamplerWrapMode U, SamplerWrapMode V, SamplerWrapMode W)
{
	VkSamplerCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.magFilter = GetVulkanSamplerFilter(Near);
	info.minFilter = GetVulkanSamplerFilter(Far);
	info.addressModeU = GetVulkanSamplerWrapMode(U);
	info.addressModeV = GetVulkanSamplerWrapMode(V);
	info.addressModeW = GetVulkanSamplerWrapMode(W);
	info.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties Props;
	vkGetPhysicalDeviceProperties(VulkanContext::Get()->GetPhysicalDeviceRef()->Get(), &Props);

	info.maxAnisotropy = Props.limits.maxSamplerAnisotropy;
	info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	info.unnormalizedCoordinates = VK_FALSE;
	info.compareEnable = VK_FALSE;
	info.compareOp = VK_COMPARE_OP_ALWAYS;
	info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	info.mipLodBias = 0.f;
	info.minLod = 0.f;
	info.maxLod = 1;

	_VKR = vkCreateSampler(VulkanContext::Get()->GetDeviceRef()->Get(), &info, nullptr, &m_Sampler);
	CHECK_HANDLE(m_Sampler, VkSampler);

	NEXUS_LOG("Vulkan", "Sampler Created");
}

Nexus::VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(VulkanContext::Get()->GetDeviceRef()->Get(), m_Sampler, nullptr);
	NEXUS_LOG("Vulkan", "Sampler Destroyed");
}
