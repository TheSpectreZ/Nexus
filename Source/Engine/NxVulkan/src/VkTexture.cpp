#include "NxVulkan/VkTexture.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkCommandQueue.h"

Nexus::VulkanTexture::VulkanTexture(const TextureSpecification& info)
	:m_Extent(Extent(info.extent.width, info.extent.height))
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	VulkanSwapchain* swapchain = VulkanSwapchain::Get();

	// Image
	{
		m_MipCount = info.mipCount;
		m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_ArrayLayerCount = 1;

		VkImageCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.extent = { info.extent.width,info.extent.height,1 };
		Info.arrayLayers = 1;
		Info.mipLevels = info.mipCount;
		Info.pQueueFamilyIndices = nullptr;
		Info.queueFamilyIndexCount = 0;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.tiling = VK_IMAGE_TILING_OPTIMAL;
		Info.samples = VK_SAMPLE_COUNT_1_BIT;
		Info.initialLayout = m_CurrentLayout;
		Info.flags = 0;

		switch (info.type)
		{
		case TextureType::TwoDim:
			Info.imageType = VK_IMAGE_TYPE_2D;
			break;
		case TextureType::ThreeDim:
			Info.imageType = VK_IMAGE_TYPE_3D;
			break;
		case TextureType::Cube:
		{
			m_ArrayLayerCount = 6;
			Info.arrayLayers = 6;
			Info.imageType = VK_IMAGE_TYPE_2D;
			Info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			break;
		}
		default:
			break;
		};

		switch (info.format)
		{
		case TextureFormat::RGBA8_SRGB:
			Info.format = VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case TextureFormat::RG16_SFLOAT:
			Info.format = VK_FORMAT_R16G16_SFLOAT;
			break;
		case TextureFormat::RG32_SFLOAT:
			Info.format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case TextureFormat::RGBA16_SFLOAT:
			Info.format = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case TextureFormat::RGBA32_SFLOAT:
			Info.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			break;
		};

		switch (info.usage)
		{
		case TextureUsage::ShaderSampled:
			Info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		case TextureUsage::StorageWrite	:
			Info.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			break;
		default:
			break;
		}

		VmaAllocationCreateInfo aInfo{};
		aInfo.usage = VMA_MEMORY_USAGE_AUTO;

		_VKR = vmaCreateImage(device->GetAllocator(), &Info, &aInfo, &m_Image, &m_Alloc, nullptr);
		CHECK_HANDLE(m_Image, VkImage);
	}

	m_Views.resize(info.mipCount);
	for (uint32_t i = 0; i < info.mipCount; i++)
	{
		VkImageViewCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.image = m_Image;
		Info.components.r = VK_COMPONENT_SWIZZLE_R;
		Info.components.g = VK_COMPONENT_SWIZZLE_G;
		Info.components.b = VK_COMPONENT_SWIZZLE_B;
		Info.components.a = VK_COMPONENT_SWIZZLE_A;
		Info.subresourceRange.layerCount = 1;
		Info.subresourceRange.levelCount = info.mipCount;
		Info.subresourceRange.baseArrayLayer = 0;
		Info.subresourceRange.baseMipLevel = 0;
		Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		
		switch (info.type)
		{
		case TextureType::TwoDim:
			Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case TextureType::ThreeDim:
			Info.viewType = VK_IMAGE_VIEW_TYPE_3D;
			break;
		case TextureType::Cube:
		{
			Info.subresourceRange.layerCount = 6;
			Info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		}
		default:
			break;
		};

		switch (info.format)
		{
		case TextureFormat::RGBA8_SRGB:
			Info.format = VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case TextureFormat::RG16_SFLOAT:
			Info.format = VK_FORMAT_R16G16_SFLOAT;
			break;
		case TextureFormat::RG32_SFLOAT:
			Info.format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case TextureFormat::RGBA16_SFLOAT:
			Info.format = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case TextureFormat::RGBA32_SFLOAT:
			Info.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			break;
		};

		_VKR = vkCreateImageView(device->Get(), &Info, nullptr, &m_Views[i]);
		CHECK_HANDLE(m_Views[i], VkImageView);
	}

	m_StagingBuffer = nullptr;
	m_StagingAlloc = nullptr;

	if (info.usage == TextureUsage::ShaderSampled) // Staging
	{
		VkDeviceSize size = (VkDeviceSize)info.extent.width * info.extent.height * 4;

		if (info.format == TextureFormat::RGBA32_SFLOAT)
			size *= sizeof(float);
		else
			size *= sizeof(uint8_t);

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
			memcpy(allocInfo.pMappedData, info.pixels, size);
		}

		if(info.now)
			VulkanCommandQueue::Get()->TransferTextureToGPU_Now(this);
		else
			VulkanCommandQueue::Get()->TransferTextureToGPU(this);

	}
	else if (info.usage == TextureUsage::StorageWrite)
	{
		VulkanCommandQueue::Get()->TransitionTextureLayout_Now(this, VK_IMAGE_LAYOUT_GENERAL, VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, PipelineBindPoint::Compute);
		m_CurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
	}

	NEXUS_LOG("Vulkan", "Texture Created");
}

Nexus::VulkanTexture::~VulkanTexture()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	if (m_StagingBuffer != nullptr)
		vmaDestroyBuffer(device->GetAllocator(), m_StagingBuffer, m_StagingAlloc);

	for(auto& v : m_Views)
		vkDestroyImageView(device->Get(), v, nullptr);

	vmaDestroyImage(device->GetAllocator(), m_Image, m_Alloc);
	NEXUS_LOG("Vulkan", "Texture Destroyed");
}

void Nexus::VulkanTexture::PrepareForRender()
{
	VulkanCommandQueue::Get()->TransitionTextureLayout_Now(this, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, PipelineBindPoint::Graphics);
	m_CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

Nexus::VulkanSampler::VulkanSampler(const SamplerSpecification& specs)
{
	VkSamplerCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.magFilter = GetVulkanSamplerFilter(specs.sampler.Near);
	info.minFilter = GetVulkanSamplerFilter(specs.sampler.Far);
	info.addressModeU = GetVulkanSamplerWrapMode(specs.sampler.U);
	info.addressModeV = GetVulkanSamplerWrapMode(specs.sampler.V);
	info.addressModeW = GetVulkanSamplerWrapMode(specs.sampler.W);
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
