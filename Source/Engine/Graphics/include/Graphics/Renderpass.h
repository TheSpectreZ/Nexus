#pragma once
#include "Build.h"
#include "Framebuffer.h"

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
			
			void SetFramebuffers(std::vector<Framebuffer>* framebuffers) { m_CurrentFramebuffers = framebuffers; }
			void SetClearValues(std::vector<VkClearValue>* clearValues) { m_CurrentClearValues = clearValues; }

			void Begin(VkCommandBuffer cmd);
			void End(VkCommandBuffer cmd);

			VkRenderPass& Get() { return m_handle; }
			VkRenderPass Get() const { return m_handle; }
		private:
			std::vector<Framebuffer>* m_CurrentFramebuffers;
			std::vector<VkClearValue>* m_CurrentClearValues;
			VkRenderPass m_handle;
		};
	}
}