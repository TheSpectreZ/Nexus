#include "nxpch.h"
#include "Renderpass.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkRenderpass.h"

Nexus::Ref<Nexus::Renderpass> Nexus::Renderpass::Create(const RenderpassSpecification& specs)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanRenderpass>(specs);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
