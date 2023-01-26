#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"

#include <vector>

namespace Nexus
{
	namespace Graphics
	{
		typedef VkDescriptorSet DescriptorSet;

		class NEXUS_GRAPHICS_API DescriptorPool
		{
		public:
			void Create(std::vector<VkDescriptorPoolSize>* PoolSizes, uint32_t MaxSet);
			void Destroy();
			VkDescriptorPool& Get() { return m_handle; }
			VkDescriptorPool Get() const { return m_handle; }
		private:
			VkDescriptorPool m_handle;
		};

		class NEXUS_GRAPHICS_API DescriptorLayout
		{
		public:
			void Create(std::vector<VkDescriptorSetLayoutBinding>* bindings);
			void Destroy();
			VkDescriptorSetLayout& Get() { return m_handle; }
			VkDescriptorSetLayout Get() const { return m_handle; }
		private:
			VkDescriptorSetLayout m_handle;
		};

		class NEXUS_GRAPHICS_API Descriptor
		{
		public:
			static VkDescriptorSet AllocateSet(DescriptorLayout* layout,DescriptorPool* pool);
			static void BindWithBuffer(VkDescriptorSet set,VkBuffer buffer,VkDeviceSize size,uint32_t binding,uint32_t arrayElm);
			static void Bind(VkCommandBuffer cmdbuffer, VkPipelineLayout layout,uint32_t setIndex,VkDescriptorSet set);
		};
	}
}
