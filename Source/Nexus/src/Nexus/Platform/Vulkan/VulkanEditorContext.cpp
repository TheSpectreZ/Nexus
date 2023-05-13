#include "nxpch.h"
#include "VulkanEditorContext.h"
#include "VkCommandQueue.h"

#include "backends/imgui_impl_vulkan.cpp"
#include "backends/imgui_impl_glfw.h"

#include "Core/Application.h"
#include "VkContext.h"
#include "VkSwapchain.h"
#include "VkRenderpass.h"
#include "VkFramebuffer.h"
#include "VkTexture.h"

Nexus::VulkanEditorContext::VulkanEditorContext(Ref<Renderpass> renderpass)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Font
	{
		ImGuiIO& Io = ImGui::GetIO();
		Io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Bold.ttf", 18.f);
		Io.FontDefault = Io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Regular.ttf", 18.f);
	}

	// Style
	{
		auto& colors = style.Colors;

		colors[ImGuiCol_WindowBg] = ImVec4{ 0.09f, 0.09f, 0.09f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	Window& window = Application::Get()->GetWindow();
	ImGui_ImplGlfw_InitForVulkan(window.handle, true);

	Ref<VulkanContext> context = VulkanContext::Get();
	Ref<VulkanSwapchain> swapchain = VulkanSwapchain::Get();

	// Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
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
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(context->GetDeviceRef()->Get(), &pool_info, nullptr, &m_Pool);
	}

	ImGui_ImplVulkan_InitInfo Info{};
	Info.Instance = context->Get()->GetInstance();
	Info.PhysicalDevice = context->GetPhysicalDeviceRef()->Get();
	Info.Device = context->GetDeviceRef()->Get();
	Info.Allocator = nullptr;
	Info.CheckVkResultFn = nullptr;
	Info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	Info.PipelineCache = VK_NULL_HANDLE;
	Info.Queue = context->GetDeviceRef()->GetGraphicsQueue();
	Info.QueueFamily = context->GetDeviceRef()->GetQueueFamilyIndices().Graphics;
	Info.Subpass = 0;
	Info.ImageCount = swapchain->GetImageCount();
	Info.MinImageCount = 2;
	Info.DescriptorPool = m_Pool;

	ImGui_ImplVulkan_Init(&Info, DynamicPointerCast<VulkanRenderpass>(renderpass)->Get());

	m_PipelineLayout = ImGui_ImplVulkan_GetBackendData()->PipelineLayout;

	{
		VkCommandPoolCreateInfo p{};
		p.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		p.flags = 0;
		p.queueFamilyIndex = context->GetDeviceRef()->GetQueueFamilyIndices().Graphics;
		p.pNext = nullptr;

		VkCommandPool pool;
		vkCreateCommandPool(context->GetDeviceRef()->Get(), &p, nullptr, &pool);

		VkCommandBufferAllocateInfo a{};
		a.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		a.pNext = nullptr;
		a.commandBufferCount = 1;
		a.commandPool = pool;
		a.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer buffer;
		vkAllocateCommandBuffers(context->GetDeviceRef()->Get(), &a, &buffer);

		VkCommandBufferBeginInfo b{};
		b.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		b.pNext = nullptr;
		b.pInheritanceInfo = nullptr;
		b.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(buffer, &b);
		ImGui_ImplVulkan_CreateFontsTexture(buffer);
		vkEndCommandBuffer(buffer);

		VkSubmitInfo s{};
		s.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		s.commandBufferCount = 1;
		s.pCommandBuffers = &buffer;

		vkQueueSubmit(context->GetDeviceRef()->GetGraphicsQueue(), 1, &s, nullptr);
		vkQueueWaitIdle(context->GetDeviceRef()->GetGraphicsQueue());

		vkDestroyCommandPool(context->GetDeviceRef()->Get(), pool, nullptr);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

Nexus::VulkanEditorContext::~VulkanEditorContext()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(VulkanContext::Get()->GetDeviceRef()->Get(), m_Pool, nullptr);
}

void Nexus::VulkanEditorContext::Start()
{
	m_CmdBuffer = VulkanCommandQueue::Get()->GetCurrentCommandBuffer();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Nexus::VulkanEditorContext::End()
{
	ImGui::Render();

	ImDrawData* data = ImGui::GetDrawData();
	if (data)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CmdBuffer);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

ImTextureID Nexus::VulkanEditorContext::MakeTextureID(Ref<Framebuffer> framebuffer, Ref<Sampler> sampler, uint32_t bufferIndex, uint32_t imageIndex)
{
	VkSampler samp = DynamicPointerCast<VulkanSampler>(sampler)->Get();
	VkImageView view = DynamicPointerCast<VulkanFramebuffer>(framebuffer)->GetAttachmentView(bufferIndex, imageIndex);

	return ImGui_ImplVulkan_AddTexture(samp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

ImTextureID Nexus::VulkanEditorContext::MakeTextureID(Ref<Texture> texture, Ref<Sampler> sampler)
{
	VkSampler samp = DynamicPointerCast<VulkanSampler>(sampler)->Get();
	VkImageView view = DynamicPointerCast<VulkanTexture>(texture)->Get();

	return ImGui_ImplVulkan_AddTexture(samp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Nexus::VulkanEditorContext::DestroyTextureID(ImTextureID Id)
{
	ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)Id);
}

void Nexus::VulkanEditorContext::BindTextureID(ImTextureID Id)
{
	VkDescriptorSet set = (VkDescriptorSet)Id;
	vkCmdBindDescriptorSets(m_CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &set, 0, nullptr);
}