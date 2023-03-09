#pragma once
#include "Renderer/Context.h"
#include "Renderer/Renderer.h"

#include "VkAssert.h"
#include "VkDevice.h"

namespace Nexus
{
	class VulkanContext : public Context
	{
	public:
		void Init() override;
		void Shut() override;
		void WaitForDevice() override;

		static Ref<VulkanContext> Get() { return DynamicPointerCast<VulkanContext>(Renderer::GetContext()); }
		
		VkInstance GetInstance() { return m_Instance; }
		VkSurfaceKHR GetSurface() { return m_Surface; }
		
		Ref<VulkanDevice> GetDeviceRef() { return m_LogicalDevice; }
		Ref<VulkanPhysicalDevice> GetPhysicalDeviceRef() { return m_PhysicalDevice; }
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanDevice> m_LogicalDevice;
	};
}
