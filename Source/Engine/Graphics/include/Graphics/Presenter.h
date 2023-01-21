#pragma once
#include "vulkan/vulkan.h"
#include "EngineSpecification.h"

#include <vector>

namespace Nexus
{
	namespace Graphics
	{
		class Presenter
		{
			static Presenter* s_Instance;
		public:
			static void Init(const EngineSpecification& specs);
			static void Shut();
		private:
			VkSwapchainKHR m_Swapchain;
			VkFormat m_SwapchainImageFormat;
			VkExtent2D m_SwapchainExtent;

			std::vector<VkImage> m_Images;
			std::vector<VkImageView> m_ImageViews;
		};
	}
}