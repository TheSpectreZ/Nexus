#include "nxpch.h"
#include "Framebuffer.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkFramebuffer.h"

Nexus::Ref<Nexus::Framebuffer> Nexus::Framebuffer::Create(const FramebufferSpecification& specs)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanFramebuffer>(specs);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
