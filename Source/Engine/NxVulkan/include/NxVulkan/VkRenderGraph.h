#pragma once
#include "NxGraphics/RenderGraph.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"

namespace Nexus
{
	class VulkanRenderTarget
	{
	public:
		VulkanRenderTarget() = default;
		~VulkanRenderTarget() = default;

		void Create(const RenderTargetSpecification& specs);
		void Destroy(VkDevice device, VmaAllocator allocator);

		VkImageView& Get(uint32_t Index) { return m_Handles[Index].view; }
	private:
		struct RTHandle
		{
			VkImage image;
			VkImageView view;
			VmaAllocation alloc;
		};
		
		std::vector<RTHandle> m_Handles;
	};

	class VulkanRenderGraphPass
	{
	public:
		VulkanRenderGraphPass() = default;
		~VulkanRenderGraphPass() = default;

		void Create(const RenderGraphPassSpecification& specs, class VulkanRenderGraph* graph);
		void Destroy(VkDevice device);

		VkRenderPass& Get() { return m_Handle; }
	private:
		VkRenderPass m_Handle;
	};

	class NEXUS_VULKAN_API VulkanRenderGraph : public RenderGraph
	{
		friend class VulkanRenderGraphPass;
	public:
		VulkanRenderGraph();
		~VulkanRenderGraph() override;

		void Bake() override;
	private:
		Ref<VulkanDevice> m_Device;
		Ref<VulkanPhysicalDevice> m_pDevice;
		
		std::unordered_map<std::string, VulkanRenderTarget> m_Attachments;
		std::unordered_map<std::string, VulkanRenderGraphPass> m_Passes;
	};
}
