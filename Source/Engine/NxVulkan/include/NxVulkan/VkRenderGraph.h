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

		void Create(const RenderTargetSpecification& specs, Extent extent);
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

	class VulkanRenderGraphFrameBuffer
	{
	public:
		VulkanRenderGraphFrameBuffer() = default;
		~VulkanRenderGraphFrameBuffer() = default;

		void Create(const RenderGraphPassSpecification& specs, VkRenderPass& pass, class VulkanRenderGraph* graph);
		void Destroy(VkDevice device);

		VkFramebuffer& Get(uint32_t Index) { return m_Handles[Index]; }
	private:
		std::vector<VkFramebuffer> m_Handles;
	};

	class VulkanRenderGraphPass
	{
		friend class VulkanGraphicsRenderPipeline;
	public:
		VulkanRenderGraphPass() = default;
		~VulkanRenderGraphPass() = default;

		void Create(const RenderGraphPassSpecification& specs, class VulkanRenderGraph* graph);
		void Destroy(VkDevice device);

		VkRenderPass& Get() { return m_Handle; }
	private:
		VkRenderPass m_Handle = nullptr;
		VulkanRenderGraphFrameBuffer m_Framebuffer;

		uint32_t m_colorAttachmentCount;
	};

	class VulkanGraphicsRenderPipeline
	{
	public:
		VulkanGraphicsRenderPipeline() = default;
		~VulkanGraphicsRenderPipeline() = default;

		void Create(const GraphicsRenderPipelineSpecification& specs, VulkanRenderGraphPass& renderPass);
		void Destroy(VkDevice device);

		VkPipeline& Get() { return m_Handle; }
	private:
		VkPipeline m_Handle = nullptr;
	};

	class NEXUS_VULKAN_API VulkanRenderGraph : public RenderGraph
	{
		friend class VulkanRenderGraphPass;
		friend class VulkanRenderGraphFrameBuffer;
		friend class VulkanGraphicsRenderPipeline;
	public:
		VulkanRenderGraph();
		~VulkanRenderGraph() override;
	private:
		void Bake() override;
		
		Ref<VulkanDevice> m_Device;
		Ref<VulkanPhysicalDevice> m_pDevice;
		
		std::unordered_map<std::string, VulkanRenderTarget> m_Attachments;
		std::unordered_map<std::string, VulkanRenderGraphPass> m_Passes;
		std::unordered_map<std::string, VulkanGraphicsRenderPipeline> m_GraphicsPipelines;
	};
}
