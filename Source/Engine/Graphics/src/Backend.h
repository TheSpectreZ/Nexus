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
			static Backend* s_Instance;
		public:
			static void Init(const EngineSpecification& specs);
			static void Shut();

			static void WaitForDevice();
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
			static VkCommandBuffer BeginSingleTimeCommands();
			static void EndSingleTimeCommands(VkCommandBuffer buffer);

			static VmaAllocator GetAllocator() { return s_Instance->m_VmaAllocator; }
			static VkDevice GetDevice() { return s_Instance->m_Device; }
			static VkSurfaceKHR GetSurface() { return s_Instance->m_Surface; }
			static VkPhysicalDevice GetPhysicalDevice() { return s_Instance->m_PhysicalDevice; }
			static VkQueue GetGraphicsQueue() { return s_Instance->m_GraphicsQueue; }
			static VkQueue GetPresentQueue() { return s_Instance->m_PresentQueue; }
			static VkSampleCountFlagBits GetMaxSampleCount() { return s_Instance->m_Msaa; }
		};

		uint32_t FindMemoryType(VkPhysicalDevice dev, uint32_t filter, VkMemoryPropertyFlags property);

		VkFormat FindSupportedFormat(VkPhysicalDevice dev, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		typedef std::vector<std::optional<uint32_t>> QueueIndexFamilies;
		QueueIndexFamilies GetQueueIndexFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	}
}