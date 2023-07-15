#pragma once
#include "NxGraphics/Renderpass.h"
#include "VkAssert.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_VULKAN_API VulkanRenderpass : public Renderpass
	{
	public:
		VulkanRenderpass(const RenderpassSpecification& specs);
		~VulkanRenderpass() override;
		
		VkRenderPass& Get() { return m_Pass; }
	private:
		VkRenderPass m_Pass;
	};
}