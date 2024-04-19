#include "NxVulkan/VkUserInterface.h"
#include "NxVulkan/VkGraphicsDevice.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

Nexus::VulkanUserInterface::VulkanUserInterface(VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice)
{
	auto vkDevice = pDevice->getDevice();

	std::vector<vk::DescriptorPoolSize> poolSizes =
	{
		{ vk::DescriptorType::eCombinedImageSampler, 1 },
	};

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.maxSets = 1;
	poolInfo.setPoolSizes(poolSizes);
	
	mDescriptorPool = vkDevice.createDescriptorPool(poolInfo);

	auto window = pDevice->getWindow();
	ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window->glfwHandle, true);

	ImGui_ImplVulkan_InitInfo Info{};
	Info.Instance = pDevice->getInstance();
	Info.PhysicalDevice = pDevice->getPhysicalDevice();
	Info.Device = vkDevice;
	Info.QueueFamily = pDevice->getGraphicsQueueFamily();
	Info.Queue = pDevice->getGraphicsQueue();
	Info.PipelineCache = nullptr;
	Info.Subpass = 0;
	Info.MinImageCount = pDevice->getImageCount();
	Info.ImageCount = pDevice->getImageCount();
	Info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	Info.Allocator = nullptr;
	Info.UseDynamicRendering = true;
	Info.ColorAttachmentFormat = (VkFormat)pDevice->getColorFormat();
	Info.DescriptorPool = mDescriptorPool;

	ImGui_ImplVulkan_Init(&Info, nullptr);

	// Upload Fonts
	{
		vk::CommandPoolCreateInfo poolInfo{};
		poolInfo.setQueueFamilyIndex(pDevice->getGraphicsQueueFamily());
		poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		auto pool = vkDevice.createCommandPool(poolInfo);

		vk::CommandBufferAllocateInfo gcInfo{};
		gcInfo.commandPool = pool;
		gcInfo.level = vk::CommandBufferLevel::ePrimary;
		gcInfo.commandBufferCount = 1;

		auto cmdBuffers = vkDevice.allocateCommandBuffers(gcInfo);
		auto cmd = cmdBuffers[0];

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		
		cmd.begin(beginInfo);
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		cmd.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.setCommandBuffers(cmd);

		auto queue = pDevice->getGraphicsQueue();
		queue.submit(submitInfo);

		vkDevice.waitIdle();

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		vkDevice.destroyCommandPool(pool);
	}

	OnWindowResize(window->width, window->height);
}

Nexus::VulkanUserInterface::~VulkanUserInterface()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	auto vkDevice = mDevice->getDevice();
	vkDevice.destroyDescriptorPool(mDescriptorPool);
}

void Nexus::VulkanUserInterface::Begin()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Nexus::VulkanUserInterface::End()
{
	auto cmd = mDevice->getCommandBuffer();

	ImGui::Render();

	mFramebuffer->Begin();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	mFramebuffer->End();

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Nexus::VulkanUserInterface::OnWindowResize(int width, int height)
{
	FramebufferAttachment attachment{
		.format = ImageFormat::eSwapchainColor,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearColorValue{0.1f, 0.1f, 0.1f, 1.f}
	};
	
	FramebufferSpecification specs{};
	specs.extent.width = width;
	specs.extent.height = height;
	specs.colorAttachments = { attachment };

	mFramebuffer = mDevice->CreateFramebuffer(specs);
}

std::pair<Nexus::Framebuffer*, uint32_t> Nexus::VulkanUserInterface::getPresentable()
{
	return { mFramebuffer.get(), 0 };
}
