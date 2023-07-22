#include "NxImGui/Context.h"

#include "backends/imgui_impl_glfw.h"

#ifdef IMGUI_IMPL_API
#undef IMGUI_IMPL_API
#define IMGUI_IMPL_API NEXUS_IMGUI_API
#endif

#include "backends/imgui_impl_vulkan.cpp"
#include "imgui.h"

#include "NxRenderEngine/GraphicsInterface.h"
#include "NxRenderEngine/Renderer.h"

#include "NxVulkan/VkCommandQueue.h"
#include "NxVulkan/VkRenderpass.h"
#include "NxVulkan/VkContext.h"

struct EditorData
{
	Nexus::RendererAPI api;
	GLFWwindow* handle;

	VkDescriptorPool pool;
	VkPipelineLayout layout;

	Nexus::Ref<Nexus::CommandQueue> queue;
	Nexus::Ref<Nexus::VulkanCommandQueue> vkqueue;

	Nexus::FramebufferSpecification fbspecs;
	Nexus::Ref<Nexus::Framebuffer> fb;
	Nexus::Ref<Nexus::Renderpass> pass;

	Nexus::Viewport Viewport;
	Nexus::Scissor Scissor;

	~EditorData()
	{
		vkqueue.reset();
		queue.reset();
		fb.reset();
		pass.reset();
		api = Nexus::RendererAPI::NONE;
		handle = nullptr;
	}
};

static EditorData* s_Data;

void NxImGui::Context::Initialize(const Nexus::Window& window, Nexus::RendererAPI api)
{
	s_Data = new EditorData;

	s_Data->api = api;
	s_Data->handle = (GLFWwindow*)window.glfwHandle;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigViewportsNoTaskBarIcon = true;

	io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Bold.ttf", 18.f);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Regular.ttf", 18.f);

	// Style
	{
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

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

	// Renderpass
	{
		std::vector<Nexus::RenderpassAttachmentDescription> attachments;
		{
			auto& color = attachments.emplace_back();
			color.type = Nexus::ImageType::Color;
			color.multiSampled = false;
			color.hdr = false;
			color.load = Nexus::ImageOperation::Clear;
			color.store = Nexus::ImageOperation::Store;
			color.initialLayout = Nexus::ImageLayout::Undefined;
			color.finalLayout = Nexus::ImageLayout::PresentSrc;
		}

		std::vector<Nexus::SubpassDescription> subpasses;
		{
			auto& subpass0 = subpasses.emplace_back();
			subpass0.ColorAttachments = { 0 };
		}

		std::vector<Nexus::SubpassDependency> subpassDependencies;
		{
			auto& dep = subpassDependencies.emplace_back();
			dep.srcSubpass = Nexus::SubpassDependency::ExternalSubpass;
			dep.dstSubpass = 0;
			dep.srcStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.dstStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput;
			dep.srcAccessFlags = Nexus::AccessFlag::None;
			dep.dstAccessFlags = Nexus::AccessFlag::ColorAttachmentWrite;
		}

		Nexus::RenderpassSpecification specs{};
		specs.attachments = &attachments;
		specs.subpasses = &subpasses;
		specs.dependencies = &subpassDependencies;

		s_Data->pass = Nexus::GraphicsInterface::CreateRenderpass(specs);
	}

	// Framebuffer
	{
		auto extent = Nexus::Module::Renderer::Get()->GetSwapchain()->GetExtent();

		auto& a1 = s_Data->fbspecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::PresentSrc;
		a1.multisampled = false;
		a1.hdr = false;

		s_Data->fbspecs.extent = extent;
		s_Data->fbspecs.renderpass = s_Data->pass;

		s_Data->fb = Nexus::GraphicsInterface::CreateFramebuffer(s_Data->fbspecs);
	}

	// Backend
	{
		ImGui_ImplGlfw_InitForVulkan(s_Data->handle, true);

		Nexus::Ref<Nexus::VulkanContext> context = DynamicPointerCast<Nexus::VulkanContext>(Nexus::Module::Renderer::Get()->GetContext());
		Nexus::Ref<Nexus::Swapchain> swapchain = Nexus::Module::Renderer::Get()->GetSwapchain();

		s_Data->vkqueue = DynamicPointerCast<Nexus::VulkanCommandQueue>(Nexus::Module::Renderer::Get()->GetCommandQueue());

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
			vkCreateDescriptorPool(context->GetDeviceRef()->Get(), &pool_info, nullptr, &s_Data->pool);
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
		Info.DescriptorPool = s_Data->pool;

		ImGui_ImplVulkan_Init(&Info, DynamicPointerCast<Nexus::VulkanRenderpass>(s_Data->pass)->Get());
		s_Data->layout = ImGui_ImplVulkan_GetBackendData()->PipelineLayout;
	}

	// Fonts
	{
		Nexus::Ref<Nexus::VulkanContext> context = DynamicPointerCast<Nexus::VulkanContext>(Nexus::Module::Renderer::Get()->GetContext());

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

	// Screen
	{
		auto Renderer = Nexus::Module::Renderer::Get();
		auto extent = Renderer->GetSwapchain()->GetExtent();

		s_Data->queue = Renderer->GetCommandQueue();

		s_Data->Viewport.x = 0.f;
		s_Data->Viewport.y = 0.f;
		s_Data->Viewport.width = (float)extent.width;
		s_Data->Viewport.height = (float)extent.height;
		s_Data->Viewport.minDepth = 0.f;
		s_Data->Viewport.maxDepth = 1.f;

		s_Data->Scissor.Extent = extent;
		s_Data->Scissor.Offset = { 0,0 };
	}
}

void NxImGui::Context::Shutdown()
{
	Nexus::Ref<Nexus::VulkanContext> context = Nexus::DynamicPointerCast<Nexus::VulkanContext>(Nexus::Module::Renderer::Get()->GetContext());

	vkDestroyDescriptorPool(context->GetDeviceRef()->Get(), s_Data->pool, nullptr);
	
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	
	ImGui::DestroyContext();

	delete s_Data;
}

void NxImGui::Context::StartFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	s_Data->queue->BeginRenderPass(s_Data->pass, s_Data->fb);
}

void NxImGui::Context::EndFrame()
{
	ImGui::Render();

	ImDrawData* data = ImGui::GetDrawData();
	if (data)
	{
		if (s_Data->api == Nexus::RendererAPI::VULKAN)
		{
			VkCommandBuffer cmd = s_Data->vkqueue->GetCurrentCommandBuffer();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		}
	}

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	s_Data->queue->EndRenderPass();
}

void NxImGui::Context::OnWindowResize(const Nexus::Extent& extent)
{
	s_Data->fbspecs.extent = extent;

	s_Data->fb.reset();
	s_Data->fb = Nexus::GraphicsInterface::CreateFramebuffer(s_Data->fbspecs);

	s_Data->Viewport.x = 0.f;
	s_Data->Viewport.y = 0.f;
	s_Data->Viewport.width = (float)extent.width;
	s_Data->Viewport.height = (float)extent.height;
	s_Data->Viewport.minDepth = 0.f;
	s_Data->Viewport.maxDepth = 1.f;

	s_Data->Scissor.Extent = extent;
	s_Data->Scissor.Offset = { 0,0 };
}

ImTextureID NxImGui::Context::CreateTextureID(Nexus::Ref<Nexus::Texture> texture, Nexus::Ref<Nexus::Sampler> sampler)
{
	VkSampler samp = Nexus::DynamicPointerCast<Nexus::VulkanSampler>(sampler)->Get();
	VkImageView view = Nexus::DynamicPointerCast<Nexus::VulkanTexture>(texture)->Get();

	return (ImTextureID)ImGui_ImplVulkan_AddTexture(samp, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

ImTextureID NxImGui::Context::CreateTextureID(void* vkImageview, Nexus::Ref<Nexus::Sampler> sampler)
{
	VkSampler samp = Nexus::DynamicPointerCast<Nexus::VulkanSampler>(sampler)->Get();
	return (ImTextureID)ImGui_ImplVulkan_AddTexture(samp, (VkImageView)vkImageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void NxImGui::Context::BindTextureID(ImTextureID Id)
{
	VkCommandBuffer cmd = s_Data->vkqueue->GetCurrentCommandBuffer();
	VkDescriptorSet set = (VkDescriptorSet)Id;
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->layout, 0, 1, &set, 0, nullptr);
}

void NxImGui::Context::DestroyTextureID(ImTextureID Id)
{
	ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)Id);
}

uint32_t NxImGui::Context::GetFrameIndex()
{
	return s_Data->vkqueue->GetFrameIndex();
}