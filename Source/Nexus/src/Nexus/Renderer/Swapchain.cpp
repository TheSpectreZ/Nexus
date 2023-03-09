#include "nxpch.h"
#include "Swapchain.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkSwapchain.h"
#include "Platform/Vulkan/VkContext.h"

Nexus::Ref<Nexus::Swapchain> Nexus::Swapchain::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanSwapchain>();
		case RenderAPIType::NONE:  return nullptr;
		default: return nullptr;
	}
}
