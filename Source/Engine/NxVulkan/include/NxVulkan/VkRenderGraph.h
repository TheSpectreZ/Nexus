#pragma once
#include "NxGraphics/RenderGraph.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkShader.h"
#include "NxVulkan/VkTexture.h"

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

	class VulkanPipelineDescriptorManager
	{
		static const uint32_t s_MaxDescriptorSetPerPoolCount = 1000;
	public:
		VulkanPipelineDescriptorManager() = default;
		~VulkanPipelineDescriptorManager() = default;

		void Create(ReflectionData& shaderReflection);
		void Destroy(VkDevice device);

		VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }
		
		void AllocateSet(VkDevice device, uint32_t set, uint64_t hashId);
		VkDescriptorSet& GetSet(uint32_t set,uint64_t hashId, uint32_t frameIndex);
	private:
		ReflectionData m_Reflection;
		uint32_t m_FrameCount = 0; 

		std::unordered_map<uint32_t, VkDescriptorSetLayout> m_Layouts;
		VkPipelineLayout m_PipelineLayout = nullptr;

		struct DescriptorSet
		{
			std::vector<VkDescriptorSet> sets;

			VkDescriptorSet& Get(uint32_t frameIndex) { return sets[frameIndex]; }
		};
		
		struct DescriptorPool
		{
			std::vector<VkDescriptorPool> pools;
			int32_t currentID = -1;

			VkDescriptorPool& Get() { return pools[currentID]; }

			std::unordered_map<uint64_t, DescriptorSet> sets;
		};
		std::unordered_map<uint32_t, DescriptorPool> m_Pools;

		void MakeDescriptorPool(VkDevice device,uint32_t set);
	};

	class VulkanGraphicsRenderPipeline
	{
	public:
		VulkanGraphicsRenderPipeline() = default;
		~VulkanGraphicsRenderPipeline() = default;

		void Create(const GraphicsRenderPipelineSpecification& specs, VulkanRenderGraphPass& renderPass, class VulkanRenderGraph* graph);
		void Destroy(VkDevice device);

		VkPipeline& Get() { return m_Handle; }
	private:
		VulkanPipelineDescriptorManager m_DescriptorManager;
		VkPipeline m_Handle = nullptr;

		std::unordered_map<std::string, uint64_t> m_RThashID;
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
		
		Ref<VulkanSampler> m_Sampler;

		std::unordered_map<std::string, VulkanRenderTarget> m_Attachments;
		std::unordered_map<std::string, VulkanRenderGraphPass> m_Passes;
		std::unordered_map<std::string, VulkanGraphicsRenderPipeline> m_GraphicsPipelines;
	};
}
