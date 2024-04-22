#include "NxVulkan/VkUserInterface.h"
#include "NxVulkan/VkGraphicsDevice.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "NxCore/Logger.h"

Nexus::VulkanUserInterface::VulkanUserInterface(VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice)
{
	auto vkDevice = pDevice->getDevice();

	
	vk::DescriptorPoolSize poolSizes[] =
	{ 
		{ vk::DescriptorType::eSampler, 1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
		{ vk::DescriptorType::eSampledImage, 1000 },
		{ vk::DescriptorType::eStorageImage, 1000 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
		{ vk::DescriptorType::eUniformBuffer, 1000 },
		{ vk::DescriptorType::eStorageBuffer, 1000 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
		{ vk::DescriptorType::eInputAttachment, 1000 }
	};

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.maxSets = 1000;
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
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

	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	samplerInfo.anisotropyEnable = vk::False;
	samplerInfo.unnormalizedCoordinates = vk::False;
	samplerInfo.compareEnable = vk::False;
	samplerInfo.mipLodBias = 0.f;
	samplerInfo.minLod = 0.f;
	samplerInfo.maxLod = 1;

	mSampler = vkDevice.createSampler(samplerInfo);

	OnWindowResize(window->width, window->height);
}

Nexus::VulkanUserInterface::~VulkanUserInterface()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	auto vkDevice = mDevice->getDevice();
	vkDevice.destroyDescriptorPool(mDescriptorPool);
	vkDevice.destroySampler(mSampler);
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

void Nexus::VulkanUserInterface::AttachViewportPresentable(Presentable presentable)
{
	mPresentable = presentable;
	CreateDescriptorSets(mPresentable);
}

void Nexus::VulkanUserInterface::RenderViewport()
{
	if (mPresentable.first == nullptr)
	{
		NEXUS_LOG("VulkanUserInterface", "Invalid viewport presentable: Make sure you attach valid presentable");
		return;
	}

	auto cmd = mDevice->getCommandBuffer();
	auto frameIndex = mDevice->getFrameIndex();

	auto vkFb = dynamic_cast<VulkanFramebuffer*>(mPresentable.first);
	vkFb->ChangeColorAttachmentLayout(cmd, vk::ImageLayout::eShaderReadOnlyOptimal, frameIndex, mPresentable.second);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));
	
	ImGui::Begin("Viewport");
	ImGui::Image(mDescriptorsets[frameIndex], ImGui::GetContentRegionAvail());
	ImGui::End();
	
	ImGui::PopStyleVar();
}

Nexus::Presentable Nexus::VulkanUserInterface::getPresentable()
{
	return { mFramebuffer.get(), 0 };
}

void Nexus::VulkanUserInterface::CreateDescriptorSets(Presentable presentable)
{
	auto vkframebuffer = dynamic_cast<VulkanFramebuffer*>(presentable.first);
	auto views = vkframebuffer->getAttachmentViews(presentable.second);
	
	for (auto& set : mDescriptorsets)
	{
		ImGui_ImplVulkan_RemoveTexture(set);
	}
	mDescriptorsets.clear();

	for (auto& v : views)
	{
		auto set = ImGui_ImplVulkan_AddTexture(mSampler, v, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		mDescriptorsets.push_back(set);
	}
}
