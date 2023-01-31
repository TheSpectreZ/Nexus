#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"

VK_DEFINE_HANDLE(VmaAllocation)

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Sampler
		{
		public:
			void Create(VkFilter Mag, VkFilter Min);
			void Destroy();
			
			VkSampler Get() { return m_sampler; }
		private:
			VkSampler m_sampler;
		};

		class NEXUS_GRAPHICS_API VertexBuffer
		{
		public:
			void Create(uint32_t count,VkDeviceSize stride,void* data);
			void Destroy();
			void Bind(VkCommandBuffer cmd);
		private:
			VkBuffer m_buffer;
			VmaAllocation m_allocation;
		};

		class NEXUS_GRAPHICS_API IndexBuffer
		{
		public:
			void Create(uint32_t count,VkDeviceSize stride,void* data);
			void Destroy();
			void Bind(VkCommandBuffer cmd);

			uint32_t GetIndexCount() { return m_indices; }
		private:
			uint32_t m_indices;
			VkBuffer m_buffer;
			VmaAllocation m_allocation;
		};

		class NEXUS_GRAPHICS_API UniformBuffer
		{
		public:
			void Create(VkDeviceSize size);
			void Destroy();
			void Update(void* data);

			VkBuffer& Get() { return m_buffer; }
			VkBuffer Get() const { return m_buffer; }
		private:
			void* m_Data;
			VkDeviceSize m_size;
			VkBuffer m_buffer;
			VmaAllocation m_allocation;
			uint32_t m_Info;
		};

		class NEXUS_GRAPHICS_API Texture2D
		{
		public:
			void Create(const std::string& filepath);
			void Create(void* pixelData,VkSampleCountFlagBits samples,VkExtent2D extent,VkFormat format);
			void Destroy();

			VkImageView Get() { return m_view; }
		private:
			VkImage m_Image;
			VkImageView m_view;
			VmaAllocation m_allocation;
		};

	}
}