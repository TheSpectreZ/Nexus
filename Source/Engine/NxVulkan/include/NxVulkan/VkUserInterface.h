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

		void AttachViewportPresentable(Presentable presentable) override;
		void RenderViewport() override;

		Presentable getPresentable() override;
	private:
		VulkanGraphicsDevice* mDevice;
		std::shared_ptr<Framebuffer> mFramebuffer;

		vk::Sampler mSampler;
		vk::DescriptorPool mDescriptorPool;
		std::vector<VkDescriptorSet> mDescriptorsets;

		Presentable mPresentable;
		void CreateDescriptorSets(Presentable presentable);
	};
}
