#include "nxpch.h"
#include "EditorContext.h"

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

#include "Core/Application.h"
#include "Platform/Vulkan/VkContext.h"
#include "Platform/Vulkan/VkSwapchain.h"

struct EditorLayerData
{
	VkDescriptorPool m_DescPool;
	VkCommandBuffer m_CurrentCommandBuffer;
};

static EditorLayerData* s_Data;

void Nexus::EditorContext::Initialize()
{
	s_Data = new EditorLayerData();

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
		vkCreateDescriptorPool(context->GetDeviceRef()->Get(), &pool_info, nullptr, &s_Data->m_DescPool);
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
	Info.DescriptorPool = s_Data->m_DescPool;

	ImGui_ImplVulkan_Init(&Info, swapchain->GetImGuiRenderpass());

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

void Nexus::EditorContext::Shutdown()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(VulkanContext::Get()->GetDeviceRef()->Get(), s_Data->m_DescPool, nullptr);
	delete s_Data;
}

void Nexus::EditorContext::StartFrame()
{
	s_Data->m_CurrentCommandBuffer = VulkanSwapchain::Get()->GetCurrentCommandBuffer();
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Nexus::EditorContext::Render()
{
	ImGui::Render();

	ImDrawData* data = ImGui::GetDrawData();
	if (data)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), s_Data->m_CurrentCommandBuffer);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
