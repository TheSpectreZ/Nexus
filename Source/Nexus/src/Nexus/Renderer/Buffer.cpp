#include "nxpch.h"
#include "Buffer.h"

#include "Renderer/RenderAPI.h"
#include "Platform/Vulkan/VkBuffer.h"

Nexus::Ref<Nexus::StaticBuffer> Nexus::StaticBuffer::Create(uint32_t size, BufferType Type, void* data)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanStaticBuffer>(size, Type, data);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::Ref<Nexus::DynamicBuffer> Nexus::DynamicBuffer::Create(uint32_t size, BufferType Type)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanDynamicBuffer>(size, Type);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::Ref<Nexus::UniformBuffer> Nexus::UniformBuffer::Create(uint32_t size)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanUniformBuffer>(size);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
