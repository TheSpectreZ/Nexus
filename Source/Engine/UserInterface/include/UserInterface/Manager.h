#pragma once
#include "Build.h"
#include "Platform/Window.h"

#include "Graphics/Descriptor.h"
#include "Graphics/Renderpass.h"
#include "Graphics/Framebuffer.h"

namespace Nexus
{
	namespace UserInterface
	{
		class NEXUS_USER_INTERFACE_API Manager
		{
			static Manager* s_Instance;
		public:
			static Manager* Get()
			{
				return s_Instance;
			}

			Manager()
			{
				s_Instance = this;
			}

			void InitWithVulkan(Platform::Window* window);
			void ShutWithVulkan();

			void StartVulkanUIFrame();
			void EndVulkanUIFrame();
			void RenderVulkanUIFrame(VkCommandBuffer cmd);

			void OnWindowResize(uint32_t width, uint32_t height);
		private:
			void MakeFramebuffers();
			std::vector<VkClearValue> clearValue;
			Graphics::DescriptorPool dPool;
			Graphics::Renderpass rPass;
			std::vector<Graphics::Framebuffer> fBuffers;
		};
	}
}


