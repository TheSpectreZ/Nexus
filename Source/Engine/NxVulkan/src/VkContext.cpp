#include "NxVulkan/VkDevice.h"
#include "NxVulkan/VkContext.h"

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
			if (strcmp(layer, al.layerName) == 0)
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

Nexus::VulkanContext* Nexus::VulkanContext::s_Instance = nullptr;

Nexus::VulkanContext::VulkanContext(const ContextCreateInfo& createInfo)
{
	m_Info = createInfo;
}

void Nexus::VulkanContext::Initialize()
{
	s_Instance = this;

	{
		uint32_t count;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		std::vector<VkLayerProperties> AvailableLayers(count);
		vkEnumerateInstanceLayerProperties(&count, AvailableLayers.data());

		NEXUS_LOG("VULKAN", "Instance Layers:");
		for (auto& l : AvailableLayers)
		{
			NEXUS_LOG("VULKAN", "\t%s", l.layerName);
		}

	}

	std::vector<const char*> InstanceLayers;

	std::vector<const char*> DeviceExtensions;
	DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	bool debugEnabled = false;
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
		//	app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
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

		std::vector<const char*> InstanceExtensions = { "VK_KHR_surface","VK_KHR_win32_surface" };

		if (debugEnabled)
			InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		info.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
		info.ppEnabledExtensionNames = InstanceExtensions.data();

		info.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
		info.ppEnabledLayerNames = InstanceLayers.data();

		_VKR = vkCreateInstance(&info, nullptr, &m_Instance);
		CHECK_HANDLE(m_Instance, VkInstance);
		NEXUS_LOG("Vulkan", "Instance Created");
	}

	// Surface
	{
		VkWin32SurfaceCreateInfoKHR Info{};
		Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		Info.pNext = nullptr;
		Info.hwnd = (HWND)m_Info.hwnd;
		Info.hinstance = (HINSTANCE)m_Info.hInstance;
		Info.flags = 0;

		_VKR = vkCreateWin32SurfaceKHR(m_Instance, &Info, nullptr, &m_Surface); 
		CHECK_HANDLE(m_Surface, VkSurfaceKHR);
		NEXUS_LOG("Vulkan", "Surface Created");
	}

	m_PhysicalDevice = CreateRef<VulkanPhysicalDevice>(DeviceExtensions);
	m_LogicalDevice = CreateRef<VulkanDevice>(m_PhysicalDevice, DeviceExtensions, InstanceLayers);
}

void Nexus::VulkanContext::Shutdown()
{
	m_LogicalDevice.reset();

	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	NEXUS_LOG("Vulkan", "Surface Destroyed");

	vkDestroyInstance(m_Instance, nullptr);
	NEXUS_LOG("Vulkan", "Instance Destroyed");

	s_Instance = nullptr;
}

void Nexus::VulkanContext::WaitForDevice()
{
	m_LogicalDevice->Wait();
}
