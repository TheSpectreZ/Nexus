#pragma once
#include "NxGraphics/Pipeline.h"
#include "VkAssert.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& Info);
		~VulkanPipeline() override;

		VkPipeline Get() { return m_Pipeline; }
	private:
		VkPipeline m_Pipeline;
	};
}

