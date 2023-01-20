#pragma once
#include "vulkan/vulkan.h"

namespace Nexus
{
	namespace Graphics
	{
		class Backend
		{
		public:
			static Backend Get()
			{
				static Backend instance;
				return instance;
			}

			void Init();
			void Shut();
		private:
			VkInstance m_Instance;
			VkPhysicalDevice m_PhysicalDevice;
			VkDevice m_Device;
		};
	}
}