#pragma once
#include "Renderer/Renderpass.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanRenderpass : public Renderpass
	{
	public:
		VulkanRenderpass(const RenderpassSpecification& specs);
		~VulkanRenderpass() override;
	private:
		VkRenderPass m_Pass;
	};
}