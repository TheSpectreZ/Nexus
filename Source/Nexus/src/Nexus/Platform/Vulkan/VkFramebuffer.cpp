#include "nxpch.h"
#include "VkFramebuffer.h"

#include "VkContext.h"
#include "VkSwapchain.h"
#include "VkRenderPass.h"

Nexus::VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specs)
{
	VkDevice device = VulkanContext::Get()->GetDeviceRef()->Get();
	Ref<VulkanSwapchain> swapchain = VulkanSwapchain::Get();

	uint32_t ImageCount = swapchain->GetImageCount();

	m_Extent = swapchain->GetImageExtent();

	// Attachments
	m_Attachments.resize(ImageCount);
	for (uint32_t i = 0; i < ImageCount; i++)
	{
		for (auto& a : specs.attachments)
		{
			auto& k = m_Attachments[i].emplace_back();

			if (a.Type != FramebufferAttachmentType::PresentSrc)
				k.Create(a);
			else
			{
				k.image = nullptr;
				k.mem = nullptr;
				k.view = swapchain->GetImageView(i);
			}

			if (i == 0)
			{
				auto& c = m_ClearValues.emplace_back();
				if (a.Type == FramebufferAttachmentType::DepthStencil)
					c = { {1.f,0.f} };
				else
					c = { {0.1f,0.1f,0.1f,1.f} };
			}
		}
	}

	// Framebuffer
	m_Framebuffer.resize(ImageCount);
	for (uint32_t i = 0; i < ImageCount; i++)
	{
		Ref<VulkanRenderpass> pass = DynamicPointerCast<VulkanRenderpass>(specs.renderpass);

		size_t count = m_Attachments[i].size();
		std::vector<VkImageView> views(count);
		for (size_t j = 0; j < count; j++)
		{
			views[j] = m_Attachments[i][j].view;
		}

		VkFramebufferCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.width = specs.extent.width;
		Info.height = specs.extent.height;
		Info.layers = 1;
		Info.renderPass = pass->Get();
		Info.attachmentCount = (uint32_t)count;
		Info.pAttachments = views.data();

		_VKR = vkCreateFramebuffer(device, &Info, nullptr, &m_Framebuffer[i]);
		CHECK_LOG_VKR;
	}
	NEXUS_LOG_TRACE("Vulkan Framebuffer Created: AttachmentCount - {0}", m_Attachments[0].size());
}

Nexus::VulkanFramebuffer::~VulkanFramebuffer()
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();

	for (uint32_t i = 0; i < m_Framebuffer.size(); i++)
	{
		for (auto& k : m_Attachments[i])
		{
			if (k.image != nullptr)
				k.Destroy(device->Get(), device->GetAllocator());
		}

		vkDestroyFramebuffer(device->Get(), m_Framebuffer[i], nullptr);
	}

}

void Nexus::VulkanFramebuffer::Attachment::Create(const FramebufferAttachmentDescription& desc)
{
	Ref<VulkanDevice> device = VulkanContext::Get()->GetDeviceRef();
	Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	Ref<VulkanSwapchain> swapchain = VulkanSwapchain::Get();

	// Image
	{
		VkImageCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.extent = { desc.extent.width,desc.extent.height,1 };
		Info.arrayLayers = 1;
		Info.flags = 0;
		Info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		Info.mipLevels = 1;
		Info.pQueueFamilyIndices = nullptr;
		Info.queueFamilyIndexCount = 0;
		Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		Info.tiling = VK_IMAGE_TILING_OPTIMAL;
		Info.imageType = VK_IMAGE_TYPE_2D;
		Info.samples = desc.multisampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;
		
		// NOTE: Usage is Kind of hardcoded to engine's use case, if in future something breaks considering checking it out

		switch (desc.Type)
		{
			case FramebufferAttachmentType::Color: 
				Info.format = swapchain->GetImageFormat();
				Info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
				break;
			case FramebufferAttachmentType::DepthStencil: 
				Info.format = gpu->GetDepthFormat(); 
				Info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; 
				break;
			case FramebufferAttachmentType::ShaderReadOnly_Color:
				Info.format = swapchain->GetImageFormat();
				Info.usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				break;
			default:
				Info.format = VK_FORMAT_UNDEFINED; 
				Info.usage = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM; 
				break;
		}

		VmaAllocationCreateInfo aInfo{};
		aInfo.usage = VMA_MEMORY_USAGE_AUTO;

		vmaCreateImage(device->GetAllocator(), &Info, &aInfo, &image, &mem, nullptr);
	}

	// View
	{
		VkImageViewCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		Info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		Info.image = image;
		Info.components.r = VK_COMPONENT_SWIZZLE_R;
		Info.components.g = VK_COMPONENT_SWIZZLE_G;
		Info.components.b = VK_COMPONENT_SWIZZLE_B;
		Info.components.a = VK_COMPONENT_SWIZZLE_A;
		Info.subresourceRange.layerCount = 1;
		Info.subresourceRange.levelCount = 1;
		Info.subresourceRange.baseArrayLayer = 0;
		Info.subresourceRange.baseMipLevel = 0;

		switch (desc.Type)
		{
		case FramebufferAttachmentType::Color:
			Info.format = swapchain->GetImageFormat();
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case FramebufferAttachmentType::DepthStencil:
			Info.format = gpu->GetDepthFormat();
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		case FramebufferAttachmentType::ShaderReadOnly_Color:
			Info.format = swapchain->GetImageFormat();
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		default:
			Info.format = VK_FORMAT_UNDEFINED;
			Info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_NONE;
			break;
		}

		vkCreateImageView(device->Get(), &Info, nullptr, &view);
	}
}

void Nexus::VulkanFramebuffer::Attachment::Destroy(VkDevice device, VmaAllocator allocator)
{
	vmaDestroyImage(allocator, image, mem);
	vkDestroyImageView(device, view, nullptr);
}
