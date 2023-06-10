#define VMA_IMPLEMENTATION
#include "NxVulkan/VkDevice.h"
#include "NxVulkan/VkContext.h"

static bool CheckExtensionAvailability(std::vector<const char*> extensions, VkPhysicalDevice device)
{
	uint32_t count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &count, availableExtensions.data());

	for (auto& ext : extensions)
	{
		bool found = false;
		for (auto& e : availableExtensions)
		{
			if (strcmp(ext, e.extensionName) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
			return false;
	}

	return true;
}

Nexus::VulkanPhysicalDevice::VulkanPhysicalDevice(const std::vector<const char*>& requiredExtensions)
{
	auto Instance = VulkanContext::Get()->GetInstance();

	uint32_t count = 0;
	vkEnumeratePhysicalDevices(Instance, &count, nullptr);
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(Instance, &count, devices.data());

	m_physicalDevice = nullptr;

	for (uint32_t i = 0; i < (uint32_t)devices.size(); i++)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(devices[i], &props);

		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			if (CheckExtensionAvailability(requiredExtensions, devices[i]))
				m_physicalDevice = devices[i];
		}

	}

	if (!m_physicalDevice)
	{
		NEXUS_LOG("Vulkan", "Discrete Physical Device Not Found");
		m_physicalDevice = devices.back();
	}

	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);

	NEXUS_LOG("Vulkan", "Physical Device Acquired : % s", m_properties.deviceName);

	// Depth Format
	{
		std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM 
		};

		m_DepthFormat = VK_FORMAT_UNDEFINED;
		for (VkFormat format : candidates)
		{
			VkFormatProperties Props;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &Props);;

			if (Props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				m_DepthFormat = format;
		}

		NEXUS_ASSERT((m_DepthFormat == VK_FORMAT_UNDEFINED), "No Suitable Depth Format Available");
	}

	// Max Sample Count
	{
		VkSampleCountFlags counts = m_properties.limits.framebufferColorSampleCounts & m_properties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT)
			m_MaxSampleCount = VK_SAMPLE_COUNT_64_BIT;
		if (counts & VK_SAMPLE_COUNT_32_BIT) 
			m_MaxSampleCount = VK_SAMPLE_COUNT_32_BIT;
		if (counts & VK_SAMPLE_COUNT_16_BIT) 
			m_MaxSampleCount = VK_SAMPLE_COUNT_16_BIT;
		if (counts & VK_SAMPLE_COUNT_8_BIT)
			m_MaxSampleCount = VK_SAMPLE_COUNT_8_BIT;
		if (counts & VK_SAMPLE_COUNT_4_BIT)
			m_MaxSampleCount = VK_SAMPLE_COUNT_4_BIT;
		if (counts & VK_SAMPLE_COUNT_2_BIT)
			m_MaxSampleCount = VK_SAMPLE_COUNT_2_BIT;
		else
			m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
	}
}

enum QueueType : int32_t
{
	QueueType_Graphics = 1, QueueType_Compute = 2, QueueType_Transfer = 4, QueueType_Present = 8
};

Nexus::VulkanDevice::QueueFamilyIndices Nexus::VulkanDevice::GetQueueFamilyIndices(VkPhysicalDevice device,int flags)
{
	VkSurfaceKHR surface = VulkanContext::Get()->GetSurface();

	uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
	std::vector<VkQueueFamilyProperties> QueueFamilyProperties(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, QueueFamilyProperties.data());

	QueueFamilyIndices indices;

	// Dedicated queue for compute
	if (flags & QueueType_Compute)
	{
		for (uint32_t i = 0; i < QueueFamilyProperties.size(); i++)
		{
			auto& queueFamilyProperties = QueueFamilyProperties[i];
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				indices.Compute = i;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	if (flags & QueueType_Transfer)
	{
		for (uint32_t i = 0; i < QueueFamilyProperties.size(); i++)
		{
			auto& queueFamilyProperties = QueueFamilyProperties[i];
			if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				indices.Transfer = i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < QueueFamilyProperties.size(); i++)
	{
		if ((flags & QueueType_Transfer) && indices.Transfer == -1)
		{
			if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
				indices.Transfer = i;
		}

		if ((flags & QueueType_Compute) && indices.Compute == -1)
		{
			if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
				indices.Compute = i;
		}

		if (flags & QueueType_Graphics)
		{
			if (QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.Graphics = i;
		}

		if ((flags & QueueType_Present) && indices.Present == -1)
		{
			VkBool32 found;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &found);

			if (found)
				indices.Present = i;
		}
	}

	return indices;
}

Nexus::VulkanDevice::VulkanDevice(Ref<VulkanPhysicalDevice> device,const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers)
{
	int32_t flags = QueueType_Graphics | QueueType_Compute | QueueType_Transfer | QueueType_Present;
	m_QueueFamilyIndices = GetQueueFamilyIndices(device->Get(), flags);

	float queuePriority = 0.f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;

	if (flags & QueueType_Graphics)
	{
		NEXUS_ASSERT((m_QueueFamilyIndices.Graphics == -1), "No Graphics Queue Available");

		auto& q = queueCreateInfo.emplace_back();
		q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		q.pNext = nullptr;
		q.pQueuePriorities = &queuePriority;
		q.queueCount = 1;
		q.queueFamilyIndex = m_QueueFamilyIndices.Compute;
		q.flags = 0;
	}

	if (flags & QueueType_Compute)
	{
		if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics)
		{
			NEXUS_ASSERT((m_QueueFamilyIndices.Compute == -1), "No Compute Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			q.pNext = nullptr;
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
			q.flags = 0;
		}
	}

	if (flags & QueueType_Transfer)
	{
		if ((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
		{
			NEXUS_ASSERT((m_QueueFamilyIndices.Transfer == -1), "No Transfer Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			q.pNext = nullptr;
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
			q.flags = 0;
		}
	}
	
	if (flags & QueueType_Present)
	{
		if ((m_QueueFamilyIndices.Present != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Present != m_QueueFamilyIndices.Compute))
		{
			NEXUS_ASSERT((m_QueueFamilyIndices.Present == -1), "No Present Queue Available");

			auto& q = queueCreateInfo.emplace_back();
			q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			q.pNext = nullptr;
			q.pQueuePriorities = &queuePriority;
			q.queueCount = 1;
			q.queueFamilyIndex = m_QueueFamilyIndices.Present;
			q.flags = 0;
		}
	}

	VkDeviceCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = 0;

	VkPhysicalDeviceFeatures Features{};
	Features.samplerAnisotropy = VK_TRUE;
	Features.sampleRateShading = VK_TRUE;
	Features.fillModeNonSolid = VK_TRUE;

	Info.pEnabledFeatures = &Features;

	Info.queueCreateInfoCount = (uint32_t)queueCreateInfo.size();
	Info.pQueueCreateInfos = queueCreateInfo.data();

	Info.enabledExtensionCount = (uint32_t)requiredExtensions.size();
	Info.ppEnabledExtensionNames = requiredExtensions.data();

	Info.enabledLayerCount = (uint32_t)requiredLayers.size();
	Info.ppEnabledLayerNames = requiredExtensions.data();

	_VKR = vkCreateDevice(device->Get(), &Info, nullptr, &m_logicalDevice);
	CHECK_HANDLE(m_logicalDevice, VkDevice);
	NEXUS_LOG("Vulkan","Logical Device Created");

	// Queues
	{
		vkGetDeviceQueue(m_logicalDevice, m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_logicalDevice, m_QueueFamilyIndices.Compute, 0, &m_ComputeQueue);
		vkGetDeviceQueue(m_logicalDevice, m_QueueFamilyIndices.Present, 0, &m_PresentQueue);
		vkGetDeviceQueue(m_logicalDevice, m_QueueFamilyIndices.Transfer, 0, &m_TransferQueue);

		NEXUS_LOG("Vulkan", "%s Queue Created, Family Index: %i", "Graphics", m_QueueFamilyIndices.Graphics);
		NEXUS_LOG("Vulkan", "%s Queue Created, Family Index: %i", "Present ", m_QueueFamilyIndices.Present);
		NEXUS_LOG("Vulkan", "%s Queue Created, Family Index: %i", "Transfer", m_QueueFamilyIndices.Transfer);
		NEXUS_LOG("Vulkan", "%s Queue Created, Family Index: %i", "Compute ", m_QueueFamilyIndices.Compute);
	}

	// Vulkan Memory Allocation
	{
		VmaAllocatorCreateInfo Info{};
		Info.instance = VulkanContext::Get()->GetInstance();
		Info.physicalDevice = device->Get();
		Info.device = m_logicalDevice;
		vkEnumerateInstanceVersion(&Info.vulkanApiVersion);

		_VKR = vmaCreateAllocator(&Info, &m_Allocator);
		CHECK_HANDLE(m_Allocator, VmaAllocator);
		NEXUS_LOG("Vulkan","Allocator Created");
	}
} 

Nexus::VulkanDevice::~VulkanDevice()
{
	vmaDestroyAllocator(m_Allocator);
	NEXUS_LOG("Vulkan", "Allocator Destroyed");

	vkDestroyDevice(m_logicalDevice, nullptr);
	NEXUS_LOG("Vulkan", "Logical Device Destroyed");
}

void Nexus::VulkanDevice::Wait()
{
	vkDeviceWaitIdle(m_logicalDevice);
}
