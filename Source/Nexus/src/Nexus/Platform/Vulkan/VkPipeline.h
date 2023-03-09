#pragma once
#include "Renderer/Pipeline.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineCreateInfo& Info);
		~VulkanPipeline() override;

		VkPipeline Get() { return m_Pipeline; }
	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_Layout;
	};
}

