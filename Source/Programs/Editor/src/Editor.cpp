#include "Editor.h"

#include "Utility/Logger.h"

#include "Platform/Manager.h"
#include "Platform/Input.h"
#include "Platform/FileDialog.h"

#include "Graphics/Engine.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

void Editor::Initialize()
{
	NEXUS_LOG_INIT;

	Platform::Initialize();

	m_Window.width = 1600;
	m_Window.height = 900;
	m_Window.title = "Nexus Editor";
	m_Window.type = Platform::Window::Type::VULKAN;

	Platform::Manager::Create_Window(m_Window);
	
	Platform::Input::SetContextWindow(m_Window);
	Platform::FileDialog::SetContextWindow(m_Window);

	Graphics::EngineSpecification specs;
	specs.targetWindow = &m_Window;

	Graphics::Engine::Get().Initialize(specs,NX_BIND_EVENT_FN(OnWindowResize));

	m_ClearValues["GRAPHICS"] = { {{0.7f,0.7f,0.7f, 1.0f}} , {1.f,0.f} };
	m_ClearValues["IMGUI"] = { {{0.07f,0.07f,0.07f, 1.0f}} };

	BuildRenderpass();
	BuildAttachment();
	BuildFramebuffer();
	
	BuildDescriptor();

	SetupImGui();

	m_ImguiSampler.Create(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 
		VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 
		VK_SAMPLER_ADDRESS_MODE_REPEAT);

	// Descriptor for Offscreen Attachment
	{
		uint32_t count = Engine::Get().GetMaxSwapchainImageCount();

		m_OffScreenDescriptor.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			VkImageView view = m_FramebufferAttachment[std::string("OFFSCREEN" + i)].Get();
			m_OffScreenDescriptor[i] = ImGui_ImplVulkan_AddTexture(m_ImguiSampler.Get(), view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}
}

void Editor::Run()
{
	while (Platform::Manager::IsOpen(m_Window))
	{
		Platform::Manager::PollEvents();

		auto cmdBuffer = Graphics::Engine::Get().StartFrame();

		m_Renderpass["GRAPHICS"].Begin(cmdBuffer);
		GraphicsCommandRecording(cmdBuffer);
		m_Renderpass["GRAPHICS"].End(cmdBuffer);
		
		m_Renderpass["IMGUI"].Begin(cmdBuffer);
		ImGuiCommandRecording(cmdBuffer);
		m_Renderpass["IMGUI"].End(cmdBuffer);

		Graphics::Engine::Get().EndFrame();
	}
}

void Editor::Shutdown()
{
	Graphics::Engine::Get().WaitForDevice();

	m_ImguiSampler.Destroy();
	ShutdownImGui();

	{
		for (auto& [k, v] : m_DescriptorPool)
			v.Destroy();

		for (auto& [k, v] : m_FramebufferAttachment)
			v.Destroy();

		for (auto& [k, v] : m_Framebuffer)
		{
			for (auto& f : v)
				f.Destroy();
		}

		for (auto& [k, v] : m_Renderpass)
			v.Destroy();
	}

	Graphics::Engine::Get().Shutdown();

	Platform::Manager::Destroy_Window(m_Window);

	Platform::Shutdown();
	NEXUS_LOG_SHUT;
}

void Editor::OnWindowResize(uint32_t width, uint32_t height)
{
	for (auto& [k, v] : m_FramebufferAttachment)
		v.Destroy();

	for (auto& [k, v] : m_Framebuffer)
	{
		for (auto& f : v)
			f.Destroy();
	}

	BuildAttachment();
	BuildFramebuffer();

	// Descriptor for Offscreen Attachment
	{
		uint32_t count = Engine::Get().GetMaxSwapchainImageCount();

		m_OffScreenDescriptor.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			m_OffScreenDescriptor[count] = ImGui_ImplVulkan_AddTexture(m_ImguiSampler.Get(), m_FramebufferAttachment[std::string("OFFSCREEN" + i)].Get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}
}

void Editor::GraphicsCommandRecording(VkCommandBuffer cmd)
{
}

void Editor::ImGuiCommandRecording(VkCommandBuffer cmd)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	
	// Docking
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		static bool d_open = true;
		ImGui::Begin("Dockspace", &d_open, window_flags);

		ImGui::PopStyleVar(2);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}

	ImGui::ShowDemoWindow();

	// Viewport
	{
		ImGui::Begin("Viewport");

		ImVec2 maxRegion = ImGui::GetContentRegionAvail();

		ImGui::Image(m_OffScreenDescriptor[Engine::Get().GetCurrentFrameIndex()], maxRegion);
		
		ImGui::End();
	}

	// Docking
	ImGui::End();

	ImGui::Render();

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Editor::BuildRenderpass()
{
	auto format = Graphics::Engine::Get().GetSwapchainImageFormat();
	auto samples = Graphics::Engine::Get().GetMaxSampleCount();

	// Graphics Renderpass
	{
		RenderpassCreateInfo Info{};
		
		// Attachments
		{
			auto& color = Info.descriptions.emplace_back();
			color.format = format;
			color.samples = samples;
			color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			auto& depth = Info.descriptions.emplace_back();
			depth.format =
				Graphics::GetSupportedFormat({
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT
					}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

			depth.samples = samples;
			depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			auto& offScreen = Info.descriptions.emplace_back();
			offScreen.format = format;
			offScreen.samples = VK_SAMPLE_COUNT_1_BIT;
			offScreen.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			offScreen.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			offScreen.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			offScreen.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			offScreen.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			offScreen.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		// Dependencies
		{
			auto& dependency = Info.dependecies.emplace_back();
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		VkAttachmentReference cRef{};
		cRef.attachment = 0;
		cRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference dRef{};
		dRef.attachment = 1;
		dRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference oRef{};
		oRef.attachment = 2;
		oRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Subpass
		{
			auto& subpass = Info.subpasses.emplace_back();
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &cRef;
			subpass.pDepthStencilAttachment = &dRef;
			subpass.pResolveAttachments = &oRef;
			
			subpass.inputAttachmentCount = 0;
			subpass.pInputAttachments = nullptr;

			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = nullptr;
		}

		m_Renderpass["GRAPHICS"].Create(Info);
		m_Renderpass["GRAPHICS"].SetClearValues(&m_ClearValues["GRAPHICS"]);
		m_Renderpass["GRAPHICS"].SetFramebuffers(&m_Framebuffer["GRAPHICS"]);
	}

	// ImGui Renderpass
	{
		RenderpassCreateInfo Info;

		// Attachments
		{
			auto& color = Info.descriptions.emplace_back();
			color.format = format;
			color.samples = VK_SAMPLE_COUNT_1_BIT;
			color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

		// Dependencies
		{
			auto& dependency = Info.dependecies.emplace_back();
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		VkAttachmentReference cRef{};
		cRef.attachment = 0;
		cRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Subpass
		{
			auto& subpass = Info.subpasses.emplace_back();
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &cRef;
			subpass.pDepthStencilAttachment = nullptr;
			subpass.inputAttachmentCount = 0;
			subpass.pInputAttachments = nullptr;
			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = nullptr; 
			subpass.pResolveAttachments = nullptr;
		}

		m_Renderpass["IMGUI"].Create(Info);
		m_Renderpass["IMGUI"].SetClearValues(&m_ClearValues["IMGUI"]);
		m_Renderpass["IMGUI"].SetFramebuffers(&m_Framebuffer["IMGUI"]);

	}
}

void Editor::BuildAttachment()
{
	VkExtent2D extent = Engine::Get().GetSwapchainImageExtent();
	uint32_t count = Engine::Get().GetMaxSwapchainImageCount();
	VkSampleCountFlagBits samples = Engine::Get().GetMaxSampleCount();
	VkFormat format = Engine::Get().GetSwapchainImageFormat();

	// Color
	{
		FramebufferAttachmentCreateInfo Info{};
		Info.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		Info.extent = extent;
		Info.format = format;
		Info.samples = samples;
		Info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		
		for (uint32_t i = 0; i < count; i++)
		{
			m_FramebufferAttachment[std::string("COLOR" + i)].Create(Info);
		}
	}

	// Depth
	{
		VkFormat DepthFormat = GetSupportedFormat(
			{
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT
			},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		FramebufferAttachmentCreateInfo Info{};
		Info.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		Info.extent = extent;
		Info.samples = samples;
		Info.format = DepthFormat;
		Info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		m_FramebufferAttachment["DEPTH"].Create(Info);
	}

	// OffScreen
	{
		FramebufferAttachmentCreateInfo Info{};
		Info.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		Info.extent = extent;
		Info.samples = VK_SAMPLE_COUNT_1_BIT;
		Info.format = format;
		Info.usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		for (uint32_t i = 0; i < count; i++)
		{
			m_FramebufferAttachment[std::string("OFFSCREEN" + i)].Create(Info);
		}
	}
}

void Editor::BuildFramebuffer()
{
	uint32_t count = Engine::Get().GetMaxSwapchainImageCount();
	VkExtent2D extent = Engine::Get().GetSwapchainImageExtent();

	// Graphics
	{
		m_Framebuffer["GRAPHICS"].resize(count);

		for (uint32_t i = 0; i < count; i++)
		{
			FramebufferCreateInfo Info{};
			Info.extent = extent;
			Info.Renderpass = m_Renderpass["GRAPHICS"].Get();

			Info.Attachments.emplace_back(m_FramebufferAttachment[std::string("COLOR" + i)].Get());
			Info.Attachments.emplace_back(m_FramebufferAttachment[std::string("DEPTH")].Get());
			Info.Attachments.emplace_back(m_FramebufferAttachment[std::string("OFFSCREEN" + i)].Get());

			m_Framebuffer["GRAPHICS"][i].Create(Info);
		}
	}

	// ImGui
	{
		m_Framebuffer["IMGUI"].resize(count);

		for (uint32_t i = 0; i < count; i++)
		{
			FramebufferCreateInfo Info{};
			Info.extent = extent;
			Info.Renderpass = m_Renderpass["IMGUI"].Get();

			Info.Attachments.emplace_back(Engine::Get().GetSwapChainImageView(i));
			
			m_Framebuffer["IMGUI"][i].Create(Info);
		}
	}
}

void Editor::BuildDescriptor()
{
	// ImGui
	{
		std::vector<VkDescriptorPoolSize> pool_sizes =
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

		m_DescriptorPool["IMGUI"].Create(&pool_sizes, 1000);
	}
}

void Editor::SetupImGui()
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

	ImGui_ImplGlfw_InitForVulkan(m_Window.handle, true);

	ImGui_ImplVulkan_InitInfo Info{};
	Info.Instance = Engine::Get().GetInstance();
	Info.PhysicalDevice = Engine::Get().GetGpuHandle();
	Info.Device = Engine::Get().GetDevice();
	Info.Allocator = nullptr;
	Info.CheckVkResultFn = nullptr;
	Info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	Info.PipelineCache = VK_NULL_HANDLE;
	Info.Queue = Engine::Get().GetGraphicsQueue();
	Info.QueueFamily = Engine::Get().GetQueueFamilyIndices().first;
	Info.Subpass = 0;
	Info.ImageCount = Engine::Get().GetMaxSwapchainImageCount();
	Info.MinImageCount = 2;
	Info.DescriptorPool = m_DescriptorPool["IMGUI"].Get();

	ImGui_ImplVulkan_Init(&Info,m_Renderpass["IMGUI"].Get());

	auto cmd = Engine::Get().BeginSingleTimeCommandRecording();
	ImGui_ImplVulkan_CreateFontsTexture(cmd);
	Engine::Get().EndSingleTimeCommandRecording(cmd);
	ImGui_ImplVulkan_DestroyFontUploadObjects();

}

void Editor::ShutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplVulkan_Shutdown();
	ImGui::DestroyContext();
}