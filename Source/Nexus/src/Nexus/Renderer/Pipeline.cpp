#include "nxpch.h"
#include "Pipeline.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkPipeline.h"

Nexus::Ref<Nexus::Pipeline> Nexus::Pipeline::Create(const PipelineCreateInfo& Info)
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanPipeline>(Info);
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
