#include "Graphics/Framebuffer.h"

#include "Backend.h"
#include "vkAssert.h"

void Nexus::Graphics::FramebufferAttachment::Create(const FramebufferAttachmentCreateInfo& Info)
{
	m_format = Info.format;

	VkImageCreateInfo i{};
	i.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	i.pNext = nullptr;
	i.arrayLayers = 1;
	i.extent = { Info.extent.width,Info.extent.height,1 };
	i.format = Info.format;
	i.flags = 0;
	i.imageType = VK_IMAGE_TYPE_2D;
	i.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	i.mipLevels = 1;
	i.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	i.samples = Info.samples;
	i.usage = Info.usage;
	i.tiling = VK_IMAGE_TILING_OPTIMAL;

	_VKR = vkCreateImage(Backend::GetDevice(), &i, nullptr, &m_image);
	CHECK_HANDLE(m_image, VkImage)

	VkMemoryRequirements req;
	vkGetImageMemoryRequirements(Backend::GetDevice(), m_image, &req);

	VkMemoryAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.allocationSize = req.size;
	info.memoryTypeIndex = FindMemoryType(Backend::GetPhysicalDevice(), req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vkAllocateMemory(Backend::GetDevice(), &info, nullptr, &m_memory);
	vkBindImageMemory(Backend::GetDevice(), m_image, m_memory, 0);

	VkImageViewCreateInfo v{};
	v.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	v.pNext = nullptr;
	v.image = m_image;
	v.flags = 0;
	v.viewType = VK_IMAGE_VIEW_TYPE_2D;
	v.format = Info.format;
	v.subresourceRange.aspectMask = Info.aspect;
	v.subresourceRange.baseArrayLayer = 0;
	v.subresourceRange.baseMipLevel = 0;
	v.subresourceRange.layerCount = 1;
	v.subresourceRange.levelCount = 1;

	_VKR = vkCreateImageView(Backend::GetDevice(), &v, nullptr, &m_view);
	CHECK_HANDLE(m_view, VkImageView)

	NEXUS_LOG_TRACE("Framebuffer Attachment Created");
}

void Nexus::Graphics::FramebufferAttachment::Destroy()
{
	vkDestroyImage(Backend::GetDevice(), m_image, nullptr);
	vkDestroyImageView(Backend::GetDevice(), m_view, nullptr);
	vkFreeMemory(Backend::GetDevice(), m_memory, nullptr);

	NEXUS_LOG_TRACE("Framebuffer Attachment Destroyed");
}

void Nexus::Graphics::Framebuffer::Create(const FramebufferCreateInfo& Info)
{
	VkFramebufferCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.height = Info.extent.height;
	info.width = Info.extent.width;
	info.layers = 1;
	info.attachmentCount = (uint32_t)Info.Attachments.size();
	info.pAttachments = Info.Attachments.data();
	info.renderPass = Info.Renderpass->Get();

	_VKR = vkCreateFramebuffer(Backend::GetDevice(), &info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkFramebuffer)
	NEXUS_LOG_TRACE("Framebuffer Created");
}

void Nexus::Graphics::Framebuffer::Destroy()
{
	vkDestroyFramebuffer(Backend::GetDevice(), m_handle, nullptr);
	NEXUS_LOG_TRACE("Framebuffer Destroyed");
}

VkFormat Nexus::Graphics::GetSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	return FindSupportedFormat(Backend::GetPhysicalDevice(), candidates, tiling, features);
}