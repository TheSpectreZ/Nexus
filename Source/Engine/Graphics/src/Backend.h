#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "Graphics/EngineSpecification.h"

#include <vector>
#include <optional>

namespace Nexus
{
	namespace Graphics
	{
		class Backend
		{
			friend class Engine;
			friend class Presenter;
		public:
			inline static Backend& Get()
			{
				static Backend instance;
				return instance;
			}

			void Init(const EngineSpecification& specs);
			void Shut();
		private:
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
			VkPhysicalDevice m_PhysicalDevice;
			VkDevice m_Device;
			VkDebugUtilsMessengerEXT m_DebugMessenger;
			VmaAllocator m_VmaAllocator;

			VkQueue m_GraphicsQueue,m_PresentQueue;
			VkSampleCountFlagBits m_Msaa;

			VkCommandPool m_CmdPool;
		public:
			VkCommandBuffer BeginSingleTimeCommands();
			void EndSingleTimeCommands(VkCommandBuffer buffer);
		};

		uint32_t FindMemoryType(VkPhysicalDevice dev, uint32_t filter, VkMemoryPropertyFlags property);

		VkFormat FindSupportedFormat(VkPhysicalDevice dev, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		typedef std::vector<std::optional<uint32_t>> QueueIndexFamilies;
		QueueIndexFamilies GetQueueIndexFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	}
}