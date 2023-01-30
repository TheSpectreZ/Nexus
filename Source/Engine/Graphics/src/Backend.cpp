#define VMA_IMPLEMENTATION

#include "Backend.h"
#include "vkAssert.h"

#include "GLFW/glfw3.h"

#include <algorithm>
#include <iostream>


static bool CheckLayersAvailability(std::vector<const char*> Layers)
{
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> AvailableLayers(count);
	vkEnumerateInstanceLayerProperties(&count, AvailableLayers.data());

	for (auto& layer : Layers)
	{
		bool found = false;
		for (auto& al : AvailableLayers)
		{
			if (strcmp(layer,al.layerName))
				found = true;
		}

		if (!found)
			return false;
	}

	return true;
}

static bool CheckExtensionAvailability(std::vector<const char*> extensions,VkPhysicalDevice device)
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
			if (strcmp(ext, e.extensionName))
				found = true;
		}

		if (!found)
			return false;
	}

	return true;
}

uint32_t Nexus::Graphics::FindMemoryType(VkPhysicalDevice dev, uint32_t filter, VkMemoryPropertyFlags property)
{
	VkPhysicalDeviceMemoryProperties props{};
	vkGetPhysicalDeviceMemoryProperties(dev, &props);

	for (uint32_t i = 0; i < props.memoryTypeCount; i++)
	{
		if ((filter & (1 << i)) && (props.memoryTypes[i].propertyFlags & property) == property)
			return i;
	}

	NEXUS_ASSERT("Failed To Find Suitable Memory Type", "Vulkan Error");

	return 0;
}

VkFormat Nexus::Graphics::FindSupportedFormat(VkPhysicalDevice dev, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties Props;
		vkGetPhysicalDeviceFormatProperties(dev, format, &Props);;

		if (tiling == VK_IMAGE_TILING_LINEAR && (Props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (Props.optimalTilingFeatures & features) == features)
			return format;
	}

	NEXUS_ASSERT("No Supported Format", "Vulkan Error");

	return VkFormat();
}

Nexus::Graphics::QueueIndexFamilies Nexus::Graphics::GetQueueIndexFamilies(VkPhysicalDevice device,VkSurfaceKHR surface)
{
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> properties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

	QueueIndexFamilies qIndices;
	qIndices.emplace_back();
	qIndices.emplace_back();

	for (uint32_t i = 0; i < (uint32_t)properties.size(); i++)
	{
		if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			qIndices[0] = i;

		VkBool32 present;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present);

		if (present)
			qIndices[1] = i;
	}

	return qIndices;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "[Validation Layer]: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;	
}

static bool debugEnabled = false;

Nexus::Graphics::Backend* Nexus::Graphics::Backend::s_Instance = nullptr;

void Nexus::Graphics::Backend::Init(const EngineSpecification& specs)
{
	s_Instance = new Backend();

	std::vector<const char*> InstanceLayers;

	std::vector<const char*> DeviceExtensions;
	DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef NEXUS_DEBUG
	debugEnabled = true;
	InstanceLayers.push_back("VK_LAYER_KHRONOS_validation");

	debugEnabled = CheckLayersAvailability(InstanceLayers);
	if (!debugEnabled)
		InstanceLayers.pop_back();
#endif // NEXUS_DEBUG

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

		uint32_t count;
		const char** ext = glfwGetRequiredInstanceExtensions(&count);
		std::vector<const char*> InstanceExtensions(ext, ext + count);

		if (debugEnabled)
			InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		info.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
		info.ppEnabledExtensionNames = InstanceExtensions.data();

		info.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
		info.ppEnabledLayerNames = InstanceLayers.data();

		_VKR = vkCreateInstance(&info, nullptr, &s_Instance->m_Instance);
		CHECK_HANDLE(s_Instance->m_Instance, VkInstance);
		NEXUS_LOG_WARN("Vulkan Instance Created");
	}

	// DebugMessenger
	if (debugEnabled)
	{
		VkDebugUtilsMessengerCreateInfoEXT Info{};
		Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		Info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		Info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		Info.pfnUserCallback = DebugCallBack;
		Info.pUserData = nullptr;

		auto fnc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance->s_Instance->m_Instance, "vkCreateDebugUtilsMessengerEXT");

		if (fnc != nullptr)
		{
			fnc(s_Instance->m_Instance, &Info, nullptr, &s_Instance->m_DebugMessenger);
			CHECK_HANDLE(s_Instance->m_DebugMessenger, VkDebugUtilsMessengerEXT);
			NEXUS_LOG_TRACE("Debug Messenger Created");
		}
		else
			NEXUS_ASSERT(1, "Vulkan Debug Messenger Extension Not Present");
	}

	// Surface
	{
		_VKR = glfwCreateWindowSurface(s_Instance->m_Instance, specs.targetWindow->handle, nullptr, &s_Instance->m_Surface);
		CHECK_HANDLE(s_Instance->m_Surface, VkSurfaceKHR);
		NEXUS_LOG_TRACE("Vulkan SurfaceKHR Created");
	}

	// Physical Device
	{
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(s_Instance->m_Instance, &count, nullptr);
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(s_Instance->m_Instance, &count, devices.data());

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

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(devices[i], &features);

			if (!features.samplerAnisotropy || (!features.sampleRateShading || !features.fillModeNonSolid))
				continue;

			if (!CheckExtensionAvailability(DeviceExtensions, devices[i]))
				continue;

			const auto& QueueFamilies = GetQueueIndexFamilies(devices[i], s_Instance->m_Surface);

			bool found = true;
			for (auto& fam : QueueFamilies)
			{
				if (!fam.has_value())
				{
					found = false;
					break;
				}
			}

			if (!found)
				continue;

			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				Ratings[i].Rate += 1;
			else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				Ratings[i].Rate += 2;
			else
				continue;
		}

		std::sort(Ratings.begin(), Ratings.end());

		if (!Ratings.back().Rate)
		{
			NEXUS_ASSERT(1, "No Suitable Physical Device Found");
		}

		uint32_t index = Ratings.back().Id;
		s_Instance->m_PhysicalDevice = devices[index];

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(s_Instance->m_PhysicalDevice, &props);
		CHECK_HANDLE(s_Instance->m_PhysicalDevice, VkPhysicalDevice);

		// MSAA
		{
			VkPhysicalDeviceProperties Props;
			vkGetPhysicalDeviceProperties(s_Instance->m_PhysicalDevice, &Props);

			VkSampleCountFlags counts = Props.limits.framebufferColorSampleCounts & Props.limits.framebufferDepthSampleCounts;

			if (counts & VK_SAMPLE_COUNT_64_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { s_Instance->m_Msaa = VK_SAMPLE_COUNT_2_BIT; }
			else { s_Instance->m_Msaa = VK_SAMPLE_COUNT_1_BIT; }

		}

		NEXUS_LOG_WARN("Physical Device Acquired: {0}", props.deviceName);
	}

	// Logical Device 
	{

		QueueIndexFamilies families = GetQueueIndexFamilies(s_Instance->m_PhysicalDevice, s_Instance->m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueInfo;

		float queuePriority = 1.f;

		for (const auto& fam : families)
		{
			auto& q = queueInfo.emplace_back();
			q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			q.pNext = nullptr;
			q.queueCount = 1;
			q.queueFamilyIndex = fam.value();
			q.pQueuePriorities = &queuePriority;
			q.flags = 0;
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

		Info.queueCreateInfoCount = static_cast<uint32_t>(queueInfo.size());
		Info.pQueueCreateInfos = queueInfo.data();

		Info.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		Info.ppEnabledExtensionNames = DeviceExtensions.data();

		Info.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
		Info.ppEnabledLayerNames = InstanceLayers.data();

		_VKR = vkCreateDevice(s_Instance->m_PhysicalDevice, &Info, nullptr, &s_Instance->m_Device);
		CHECK_HANDLE(s_Instance->m_Device, VkDevice);
		NEXUS_LOG_WARN("Logical Device Created");

		vkGetDeviceQueue(s_Instance->m_Device, families.front().value(), 0, &s_Instance->m_GraphicsQueue);
		CHECK_HANDLE(s_Instance->m_GraphicsQueue, VkQueue);
		NEXUS_LOG_TRACE("Graphics Queue Acquired");

		vkGetDeviceQueue(s_Instance->m_Device, families.back().value(), 0, &s_Instance->m_PresentQueue);
		CHECK_HANDLE(s_Instance->m_PresentQueue, VkQueue);
		NEXUS_LOG_TRACE("Present Queue Acquired");
	}

	// Vulkan Memory Allocator
	{
		VmaAllocatorCreateInfo Info{};
		Info.instance = s_Instance->m_Instance;
		Info.physicalDevice = s_Instance->m_PhysicalDevice;
		Info.device = s_Instance->m_Device;
		vkEnumerateInstanceVersion(&Info.vulkanApiVersion);

		_VKR = vmaCreateAllocator(&Info, &s_Instance->m_VmaAllocator);
		CHECK_LOG_VKR
		NEXUS_LOG_DEBUG("vmaAllocator Created")
	}

	// Command Pool
	{
		QueueIndexFamilies fam = GetQueueIndexFamilies(s_Instance->m_PhysicalDevice, s_Instance->m_Surface);
		
		VkCommandPoolCreateInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		Info.pNext = nullptr;
		Info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		Info.queueFamilyIndex = fam[0].value();

		vkCreateCommandPool(s_Instance->m_Device, &Info, nullptr, &s_Instance->m_CmdPool);
	}

}

void Nexus::Graphics::Backend::Shut()
{
	vkDeviceWaitIdle(s_Instance->m_Device);
	
	vmaDestroyAllocator(s_Instance->m_VmaAllocator);

	vkDestroyCommandPool(s_Instance->m_Device, s_Instance->m_CmdPool, nullptr);

	vkDestroyDevice(s_Instance->m_Device, nullptr);
	vkDestroySurfaceKHR(s_Instance->m_Instance, s_Instance->m_Surface, nullptr);

	if (debugEnabled)
	{
		auto fnc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance->m_Instance, "vkDestroyDebugUtilsMessengerEXT");
		if (fnc != NULL)
			fnc(s_Instance->m_Instance, s_Instance->m_DebugMessenger, nullptr);
	}

	vkDestroyInstance(s_Instance->m_Instance, nullptr);

	NEXUS_LOG_WARN("Vulkan Backend Shut");

	delete s_Instance;
}

void Nexus::Graphics::Backend::WaitForDevice()
{
	vkDeviceWaitIdle(s_Instance->m_Device);
}

VkCommandBuffer Nexus::Graphics::Backend::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	Info.commandPool = s_Instance->m_CmdPool;
	Info.commandBufferCount = 1;

	VkCommandBuffer buffer;
	vkAllocateCommandBuffers(s_Instance->m_Device, &Info, &buffer);

	VkCommandBufferBeginInfo begin{};
	begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin.pNext = nullptr;
	begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	begin.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(buffer, &begin);
	
	return buffer;
}

void Nexus::Graphics::Backend::EndSingleTimeCommands(VkCommandBuffer buffer)
{
	vkEndCommandBuffer(buffer);

	VkSubmitInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Info.pNext = nullptr;
	Info.commandBufferCount = 1;
	Info.pCommandBuffers = &buffer;

	vkQueueSubmit(s_Instance->m_GraphicsQueue, 1, &Info, VK_NULL_HANDLE);
	vkQueueWaitIdle(s_Instance->m_GraphicsQueue);
}
