#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"

#include <string>
#include <vector>

namespace Nexus
{
	namespace Graphics
	{

		class NEXUS_GRAPHICS_API PipelineLayout
		{
		public:
			void Create(VkDescriptorSetLayout* layout, uint32_t layoutCount, VkPushConstantRange* ranges, uint32_t rangeCount);
			void Destroy();

			VkPipelineLayout& Get() { return m_handle; }
			VkPipelineLayout Get() const { return m_handle; }
		private:
			VkPipelineLayout m_handle;
		};

		struct NEXUS_GRAPHICS_API PipelineCreateInfo
		{
			PipelineLayout* layout;

			VkPipeline basePipeline;
			int32_t basePipelineIndex;
			VkRenderPass renderPass;
			uint32_t subPassIndex;

			VkPrimitiveTopology topology;
			VkSampleCountFlagBits samples;
			
			VkPolygonMode polygonMode;
			VkCullModeFlagBits cullMode;
			VkFrontFace frontFace;

			VkPipelineColorBlendStateCreateInfo colorBlend;
			VkPipelineDepthStencilStateCreateInfo depthStencil;

			std::vector<VkDynamicState> dynamicStates;
			std::vector<VkVertexInputAttributeDescription> vertexAttributes;
			std::vector<VkVertexInputBindingDescription> vertexBindings;
			
			std::vector<std::pair<std::string, VkShaderStageFlagBits>> ShaderPaths;

			VkViewport* pViewports;
			uint32_t viewportCount;
			
			VkRect2D* pScissors;
			uint32_t scissorCount;
		};

		class NEXUS_GRAPHICS_API GraphicsPipeline
		{
		public:
			void Create(const PipelineCreateInfo& Info);
			void Destroy();
		private:
			VkPipeline m_handle;
		};
	}
}
