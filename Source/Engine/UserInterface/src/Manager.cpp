#include "UserInterface/Manager.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

#include "Graphics/Engine.h"

Nexus::UserInterface::Manager* Nexus::UserInterface::Manager::s_Instance = nullptr;

void Nexus::UserInterface::Manager::InitWithVulkan(Platform::Window* window)
{
	using namespace Graphics;

	// Descriptor Pool
	{
		std::vector<VkDescriptorPoolSize> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		dPool.Create(&sizes, 1000 * sizes.size());
	}

	// Renderpass
	{
		RenderpassCreateInfo Info{};

		auto& colorAttachment = Info.descriptions.emplace_back();
		colorAttachment.format = Engine::Get().GetSwapchainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorRef{};
		colorRef.attachment = 0;
		colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto& subpass = Info.subpasses.emplace_back();
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorRef;

		auto& dependency = Info.dependecies.emplace_back();
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		rPass.Create(Info);
	}
	MakeFramebuffers();
	
	ImGui_ImplGlfw_InitForVulkan(window->handle, true);

	ImGui_ImplVulkan_InitInfo Info{};
	Info.Instance = Engine::Get().GetInstance();
	Info.PhysicalDevice = Engine::Get().GetGpuHandle();
	Info.Device = Engine::Get().GetDevice();
	Info.Allocator = nullptr;
	Info.CheckVkResultFn = nullptr;
	Info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	Info.PipelineCache = nullptr;
	Info.Queue = Engine::Get().GetGraphicsQueue();
	Info.QueueFamily = Engine::Get().GetQueueFamilyIndices().first;
	Info.Subpass = 0;
	Info.ImageCount = Engine::Get().GetMaxSwapchainImageCount();
	Info.MinImageCount = 2;
	Info.DescriptorPool = dPool.Get();
	
	ImGui_ImplVulkan_Init(&Info, rPass.Get());
	
	auto buffer = Engine::Get().BeginSingleTimeCommandRecording();
	ImGui_ImplVulkan_CreateFontsTexture(buffer);
	Engine::Get().EndSingleTimeCommandRecording(buffer);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Nexus::UserInterface::Manager::ShutWithVulkan()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	dPool.Destroy();

	for (auto& f : fBuffers)
		f.Destroy();
	rPass.Destroy();
}

void Nexus::UserInterface::Manager::StartVulkanUIFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
}

void Nexus::UserInterface::Manager::EndVulkanUIFrame()
{
	ImGui::Render();
}

void Nexus::UserInterface::Manager::RenderVulkanUIFrame(VkCommandBuffer cmd)
{
	rPass.Begin(cmd);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	rPass.End(cmd);
}

void Nexus::UserInterface::Manager::OnWindowResize(uint32_t width, uint32_t height)
{
	for (auto& f : fBuffers)
		f.Destroy();
	
	MakeFramebuffers();
}

void Nexus::UserInterface::Manager::MakeFramebuffers()
{
	uint32_t count = Graphics::Engine::Get().GetMaxSwapchainImageCount();
	fBuffers.resize(count);

	Graphics::FramebufferCreateInfo Info{};
	Info.Renderpass = rPass.Get();
	Info.extent = Graphics::Engine::Get().GetSwapchainImageExtent();
	
	for (uint32_t i = 0; i < count; i++)
	{
		Info.Attachments = { Graphics::Engine::Get().GetSwapChainImageView(i) };
		fBuffers[i].Create(Info);
	}
}
