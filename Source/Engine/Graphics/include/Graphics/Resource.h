#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"

namespace Nexus
{
	namespace Graphics
	{
		class NEXUS_GRAPHICS_API Buffer
		{
		public:
			void Create(VkBufferUsageFlagBits usage, VkDeviceSize size);
			void Destroy();

			static void CopyBuffer(VkCommandBuffer buffer,Buffer& src, Buffer& dst, VkDeviceSize size);

			VkBuffer& Get() { return m_handle; }
			VkBuffer Get() const { return m_handle; }

			VkDeviceSize GetSize() { return m_size; }
		private:
			VkDeviceSize m_size;
			VkBuffer m_handle;
		};

		class NEXUS_GRAPHICS_API Memory
		{
		public:
			void Allocate(Buffer& buffer, VkMemoryPropertyFlags flags);
			void Free();

			void Map();
			void Update(void* data);
			void UnMap();
		private:
			VkDeviceMemory m_handle;
			VkDeviceSize m_size;
			void* m_data;
		};
	}
}
