#pragma once
#include "NxVulkan/VkAssert.h"
#include "NxGraphics/Context.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

typedef struct VkInstance_T* VkInstance;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Nexus
{
	class VulkanDevice;
	class VulkanPhysicalDevice;

	class NEXUS_VULKAN_API VulkanContext : public Context
	{
		static VulkanContext* s_Instance;
	public:
		VulkanContext(const ContextCreateInfo& Info);
		~VulkanContext() = default;

		void Initialize() override;
		void Shutdown() override;
		void WaitForDevice() override;

		static VulkanContext* Get() { return s_Instance; }

		VkInstance GetInstance() { return m_Instance; }
		VkSurfaceKHR GetSurface() { return m_Surface; }

		Ref<VulkanDevice> GetDeviceRef() { return m_LogicalDevice; }
		Ref<VulkanPhysicalDevice> GetPhysicalDeviceRef() { return m_PhysicalDevice; }
	private:
		ContextCreateInfo m_Info;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanDevice> m_LogicalDevice;
	};
}
