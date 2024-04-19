#pragma once
#include "NxGraphics/UserInterface.h"
#include "NxVulkan/VkFramebuffer.h"

namespace Nexus
{
	class VulkanUserInterface : public UserInterface
	{
	public:
		VulkanUserInterface(class VulkanGraphicsDevice* pDevice);
		~VulkanUserInterface() override;

		void Begin() override;
		void End() override;

		void OnWindowResize(int width, int height) override;

		std::pair<Framebuffer*, uint32_t> getPresentable() override;
	private:
		VulkanGraphicsDevice* mDevice;
		vk::DescriptorPool mDescriptorPool;
		
		std::shared_ptr< Framebuffer> mFramebuffer;
	};
}
