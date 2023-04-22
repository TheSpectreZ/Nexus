#include "nxpch.h"
#include "TransferCommandQueue.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkTransferCommandQueue.h"

Nexus::Ref<Nexus::TransferCommandQueue> Nexus::TransferCommandQueue::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanTransferCommandQueue>();
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
