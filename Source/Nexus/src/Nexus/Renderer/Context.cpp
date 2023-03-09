#include "nxpch.h"
#include "Context.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkContext.h"

Nexus::Ref<Nexus::Context> Nexus::Context::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanContext>();
		case RenderAPIType::NONE:  return nullptr;
		default: return nullptr;
	}
}
