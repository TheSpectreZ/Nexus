#include "nxpch.h"
#include "RenderCommandQueue.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkRenderCommandQueue.h"

Nexus::Ref<Nexus::RenderCommandQueue> Nexus::RenderCommandQueue::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanRenderCommandQueue>();	
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
