#pragma once
#include "VkAssert.h"

namespace Nexus
{
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(const std::vector<const char*>& requiredExtensions);

		VkPhysicalDevice Get() { return m_physicalDevice; }

		VkFormat GetDepthFormat() { return m_DepthFormat; }
		VkSampleCountFlagBits GetMaxSampleCount() { return m_MaxSampleCount; }
		VkPhysicalDeviceProperties GetProperties() { return m_properties; }
	private:
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_properties;

		VkFormat m_DepthFormat;
		VkSampleCountFlagBits m_MaxSampleCount;
	};

	class VulkanDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Present = -1;
			int32_t Transfer = -1;
			int32_t Compute = -1;
		};
	public:
		VulkanDevice(Ref<VulkanPhysicalDevice> device, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers);
		~VulkanDevice();
			
		void Wait();

		VkDevice Get() { return m_logicalDevice; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetComputeQueue() { return m_ComputeQueue; }
		VkQueue GetTransferQueue() { return m_TransferQueue; }
		VkQueue GetPresentQueue() { return m_PresentQueue; }
		VmaAllocator GetAllocator() { return m_Allocator; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
	private:
		VkDevice m_logicalDevice;
		VkQueue m_GraphicsQueue, m_ComputeQueue, m_TransferQueue, m_PresentQueue;
		VmaAllocator m_Allocator;

		QueueFamilyIndices m_QueueFamilyIndices;
		QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice device,int flags);
	};
}