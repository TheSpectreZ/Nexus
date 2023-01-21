#include "Graphics/Backend.h"
#include "vkAssert.h"

#include "GLFW/glfw3.h"

#include <vector>
#include <set>
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

using QueueIndexFamilies = std::vector<std::optional<uint32_t>>;
static QueueIndexFamilies GetQueueIndexFamilies(VkPhysicalDevice device,VkSurfaceKHR surface)
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

#ifndef NEXUS_DEBUG
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
		_VKR = glfwCreateWindowSurface(s_Instance->m_Instance, specs.windowHandle, nullptr, &s_Instance->m_Surface);
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
			
			const auto& QueueFamilies = GetQueueIndexFamilies(devices[i],s_Instance->m_Surface);

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
		
		if(!Ratings.back().Rate)
		{
			NEXUS_ASSERT(1, "No Suitable Physical Device Found");
		}

		uint32_t index = Ratings.back().Id;
		s_Instance->m_PhysicalDevice = devices[index];

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(s_Instance->m_PhysicalDevice, &props);

		CHECK_HANDLE(s_Instance->m_PhysicalDevice, VkPhysicalDevice);
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
}

void Nexus::Graphics::Backend::Shut()
{
	vkDeviceWaitIdle(s_Instance->m_Device);

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