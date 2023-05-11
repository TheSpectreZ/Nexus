#include "nxpch.h"
#include "CommandQueue.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkCommandQueue.h"

Nexus::Ref<Nexus::CommandQueue> Nexus::CommandQueue::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanCommandQueue>();
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
