#pragma once
#include "vulkan/vulkan.h"
#include "EngineSpecification.h"

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
		private:
			VkInstance m_Instance;
			VkSurfaceKHR m_Surface;
			VkPhysicalDevice m_PhysicalDevice;
			VkDevice m_Device;
			VkDebugUtilsMessengerEXT m_DebugMessenger;

			VkQueue m_GraphicsQueue,m_PresentQueue;
		public:
			static VkDevice GetDevice() { return s_Instance->m_Device; }
			static VkSurfaceKHR GetSurface() { return s_Instance->m_Surface; }
			static VkPhysicalDevice GetPhysicalDevice() { return s_Instance->m_PhysicalDevice; }
			static VkQueue GetGraphicsQueue() { return s_Instance->m_GraphicsQueue; }
			static VkQueue GetPresentQueue() { return s_Instance->m_PresentQueue; }
		};

		typedef std::vector<std::optional<uint32_t>> QueueIndexFamilies;
		QueueIndexFamilies GetQueueIndexFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	}
}