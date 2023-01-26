#pragma once
#include "Build.h"
#include "vulkan/vulkan.h"
#include <vector>

namespace Nexus
{
	namespace Graphics
	{
		struct NEXUS_GRAPHICS_API RenderpassCreateInfo
		{
			std::vector<VkAttachmentDescription> descriptions;
			std::vector<VkSubpassDependency> dependecies;
			std::vector<VkSubpassDescription> subpasses;
		};

		class NEXUS_GRAPHICS_API Renderpass
		{
		public:
			void Create(const RenderpassCreateInfo& Info);
			void Destroy();
			VkRenderPass& Get() { return m_handle; }

			VkRenderPass Get() const { return m_handle; }
		private:
			VkRenderPass m_handle;
		};
	}
}