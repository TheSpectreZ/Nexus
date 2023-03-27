#include "nxpch.h"
#include "ShaderResource.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkShaderResource.h"

Nexus::Ref<Nexus::ShaderResourceHeapLayout> Nexus::ShaderResourceHeapLayout::Create(const std::vector<ShaderResouceHeapLayoutBinding>& Bindings)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanShaderResourceHeapLayout>(Bindings);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::Ref<Nexus::ShaderResourcePool> Nexus::ShaderResourcePool::Create(Ref<ShaderResourceHeapLayout> layout, uint32_t maxResourceCount)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanShaderResourcePool>(layout,maxResourceCount);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}