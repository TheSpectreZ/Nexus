#include "Graphics/Backend.h"
#include "vkAssert.h"

#include "GLFW/glfw3.h"

#include <vector>
#include <algorithm>

void Nexus::Graphics::Backend::Init()
{
	std::vector<const char*> Layers;
	Layers.push_back("VK_LAYER_KHRONOS_validation");

	std::vector<const char*> Extensions;
	Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// Instance
	{
		VkApplicationInfo app{};
		app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app.pNext = nullptr;
		app.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		app.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		app.pEngineName = "NexusEngine";
		app.pApplicationName = "NexusApp";

		vkEnumerateInstanceVersion(&app.apiVersion);

		VkInstanceCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pNext = nullptr;
		info.pApplicationInfo = &app;
		info.flags = 0;

		info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&info.enabledExtensionCount);

		info.enabledLayerCount = 0;
		info.ppEnabledLayerNames = nullptr;

		_VKR = vkCreateInstance(&info, nullptr, &m_Instance);
		CHECK_HANDLE(m_Instance, VkInstance);
		NEXUS_LOG_WARN("Vulkan Instance Created");
	}

	// Physical Device
	{
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(m_Instance, &count, nullptr);
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(m_Instance, &count, devices.data());

		struct DeviceRate
		{
			uint32_t Id, Rate;
			std::string name;

			bool operator==(DeviceRate& other)
			{
				return Rate == other.Rate;
			}
			
			bool operator<(DeviceRate& other)
			{
				return Rate < other.Rate;
			}
			
			bool operator>(DeviceRate& other)
			{
				return Rate > other.Rate;
			}
		};

		std::vector<DeviceRate> Ratings(count);

		for (uint32_t i = 0; i < (uint32_t)devices.size(); i++)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(devices[i], &props);

			Ratings[i] = { i,0,props.deviceName };

			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				Ratings[i].Rate += 1;
			else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				Ratings[i].Rate += 2;
			else
				continue;
			
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(devices[i], &features);

			if (!features.samplerAnisotropy || ( !features.sampleRateShading || !features.fillModeNonSolid) )
			{
				Ratings[i].Rate = 0;
				continue;
			}
		}

		std::sort(Ratings.begin(), Ratings.end());
		
		if(!Ratings.back().Rate)
		{
			NEXUS_ASSERT(1, "No Suitable Physical Device Found");
		}

		uint32_t index = Ratings.back().Id;
		m_PhysicalDevice = devices[index];

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);

		CHECK_HANDLE(m_PhysicalDevice, VkPhysicalDevice);
		NEXUS_LOG_WARN("Physical Device Acquired: {0}", props.deviceName);
	}

	// Logical Device
	{
		VkDeviceCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = 0;
		
		VkPhysicalDeviceFeatures Features{};
		Features.samplerAnisotropy = VK_TRUE;
		Features.sampleRateShading = VK_TRUE;
		Features.fillModeNonSolid = VK_TRUE;

		Info.pEnabledFeatures = &Features;

		Info.queueCreateInfoCount = 0;
		Info.pQueueCreateInfos = nullptr;

		Info.enabledExtensionCount = 0;
		Info.ppEnabledExtensionNames = nullptr;

		Info.enabledLayerCount = 0;
		Info.ppEnabledLayerNames = nullptr;

		_VKR = vkCreateDevice(m_PhysicalDevice, &Info, nullptr, &m_Device);
		CHECK_HANDLE(m_Device, VkDevice);
		NEXUS_LOG_WARN("Logical Device Created");
	}
}

void Nexus::Graphics::Backend::Shut()
{
	vkDestroyInstance(m_Instance, nullptr);

	NEXUS_LOG_WARN("Vulkan Backend Shut");
}