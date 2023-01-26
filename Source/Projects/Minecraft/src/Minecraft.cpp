#include "Minecraft.h"
#include "Graphics/Presenter.h"
#include "glm/glm.hpp"

#include "Platform/Input.h"
#include "Platform/Manager.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

using namespace Nexus;
using namespace Nexus::Graphics;

void Minecraft::OnAttach()
{
	CreateAttachments();
	CreateRenderpasses();
	CreateFramebuffers();
	CreateDescriptors();
	CreatePipelines();

	clearValue = { { {0.5f,0.33f,0.62f,1.f} }, {{1.f,0.f} } };

	auto extent = Presenter::GetImageExtent();
	Controller.AttachCamera(&cam);
	Controller.SetProjection(extent.width, extent.height, 45.f, 0.1f, 100.f);
}

void Minecraft::OnUpdate()
{
	UpdateCamera();
}

void Minecraft::OnRender()
{
	VkCommandBuffer cmdBuffer = Presenter::GetCommandBuffer();

	Presenter::BeginRenderpass(cmdBuffer, renderpass, Framebuffers[Presenter::GetFrameIndex()], clearValue);
	
	Presenter::EndRenderpass(cmdBuffer);
}

void Minecraft::OnDetach()
{
	pipeline.Destroy();
	pipelineLayout.Destroy();

	descriptorLayout.Destroy();
	descriptorPool.Destroy();

	for (auto& f : Framebuffers)
		f.Destroy();

	for (auto& a : ColorAttachments)
		a.Destroy();

	DepthAttachment.Destroy();

	renderpass.Destroy();
}

void Minecraft::OnCallback()
{
	for (auto& f : Framebuffers)
		f.Destroy();

	for (auto& a : ColorAttachments)
		a.Destroy();

	DepthAttachment.Destroy();

	CreateAttachments();
	CreateFramebuffers();

	auto extent = Presenter::GetImageExtent();
	Controller.SetProjection(extent.width, extent.height, 45.f, 0.1f, 100.f);
}

void Minecraft::CreateAttachments()
{
	FramebufferAttachmentCreateInfo Info{};
	Info.extent = Presenter::GetImageExtent();
	Info.samples = Presenter::GetImageMaxSamples();

	// Color Attachments
	{
		Info.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		Info.format = Presenter::GetImageFormat();
		Info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		ColorAttachments.resize(Presenter::GetMaxImageCount());
		for (uint32_t i = 0; i < Presenter::GetMaxImageCount(); i++)
		{
			ColorAttachments[i].Create(Info);
		}
	}

	// Depth Attachment
	{
		Info.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		Info.format = GetSupportedFormat({ VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		Info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		DepthAttachment.Create(Info);
	}
}

void Minecraft::CreateFramebuffers()
{
	FramebufferCreateInfo Info{};
	Info.extent = Presenter::GetImageExtent();
	Info.Renderpass = &renderpass;

	Info.Attachments.resize(3);

	Info.Attachments[1] = DepthAttachment.Get();

	Framebuffers.resize(Presenter::GetMaxImageCount());
	for (uint32_t i = 0; i < Presenter::GetMaxImageCount(); i++)
	{
		Info.Attachments[0] = ColorAttachments[i].Get();
		Info.Attachments[2] = Presenter::GetImageView(i);

		Framebuffers[i].Create(Info);
	}
}

void Minecraft::CreateRenderpasses()
{
	{
		RenderpassCreateInfo Info;

		// Color Attachment
		{
			auto& color = Info.descriptions.emplace_back();
			color.format = Presenter::GetImageFormat();
			color.samples = Presenter::GetImageMaxSamples();
			color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		// Depth Attachment
		{
			auto& depth = Info.descriptions.emplace_back();
			depth.format = DepthAttachment.GetFormat();
			depth.samples = Presenter::GetImageMaxSamples();
			depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		// Resolve Attachment
		{
			auto& resolve = Info.descriptions.emplace_back();
			resolve.format = Presenter::GetImageFormat();
			resolve.samples = VK_SAMPLE_COUNT_1_BIT;
			resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

		auto& Dependency = Info.dependecies.emplace_back();
		{
			Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			Dependency.dstSubpass = 0;
			Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			Dependency.srcAccessMask = 0;
			Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		auto& SubpassDescription = Info.subpasses.emplace_back();
		{
			VkAttachmentReference cRef{}, dRef{}, rRef{};
			cRef.attachment = 0;
			cRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			dRef.attachment = 1;
			dRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			rRef.attachment = 2;
			rRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			SubpassDescription.colorAttachmentCount = 1;
			SubpassDescription.pColorAttachments = &cRef;
			SubpassDescription.pDepthStencilAttachment = &dRef;
			SubpassDescription.pResolveAttachments = &rRef;
		}

		renderpass.Create(Info);
	}
}

void Minecraft::CreateDescriptors()
{
	// Pool
	{
		std::vector<VkDescriptorPoolSize> sizes = { 
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1} 
		};

		descriptorPool.Create(&sizes, 1);
	}

	// Layout
	{
		std::vector<VkDescriptorSetLayoutBinding> layouts =
		{
			{0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1,VK_SHADER_STAGE_VERTEX_BIT ,nullptr}
		};

		descriptorLayout.Create(&layouts);
	}

	// Sets
	{
		descriptorSet = Descriptor::AllocateSet(&descriptorLayout, &descriptorPool);
	}
}

void Minecraft::CreatePipelines()
{
	// Pipeline layout
	{
		pipelineLayout.Create(&descriptorLayout.Get(), 1, nullptr, 0);
	}

	// Pipeline
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		PipelineCreateInfo Info{};

		Info.colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		Info.colorBlend.logicOpEnable = VK_FALSE;
		Info.colorBlend.logicOp = VK_LOGIC_OP_COPY;
		Info.colorBlend.attachmentCount = 1;
		Info.colorBlend.pAttachments = &colorBlendAttachment;
		Info.colorBlend.blendConstants[0] = 0.0f;
		Info.colorBlend.blendConstants[1] = 0.0f;
		Info.colorBlend.blendConstants[2] = 0.0f;
		Info.colorBlend.blendConstants[3] = 0.0f;

		Info.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		Info.depthStencil.depthTestEnable = VK_TRUE;
		Info.depthStencil.depthWriteEnable = VK_TRUE;
		Info.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		Info.depthStencil.depthBoundsTestEnable = VK_FALSE;
		Info.depthStencil.minDepthBounds = 0.0f;
		Info.depthStencil.maxDepthBounds = 1.0f;
		Info.depthStencil.stencilTestEnable = VK_FALSE;
		Info.depthStencil.front = {};
		Info.depthStencil.back = {};

		Info.basePipeline = VK_NULL_HANDLE;
		Info.basePipelineIndex = 0;
		Info.layout = &pipelineLayout;
		
		Info.cullMode = VK_CULL_MODE_BACK_BIT;
		Info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		Info.polygonMode = VK_POLYGON_MODE_FILL;
		Info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		Info.samples = Presenter::GetImageMaxSamples();

		Info.renderPass = renderpass.Get();
		Info.subPassIndex = 0;

		Info.dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_CULL_MODE,
			VK_DYNAMIC_STATE_FRONT_FACE,
		};

		Info.viewportCount = 1;
		Info.pViewports = nullptr;
		Info.scissorCount = 1;
		Info.pScissors = nullptr;

		Info.vertexBindings = { {0,sizeof(Vertex),VK_VERTEX_INPUT_RATE_VERTEX} };

		Info.vertexAttributes =
		{
			{0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Vertex::position) },
			{1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Vertex::color) },
		};

		Info.ShaderPaths =
		{
			{"res/shaders/Vert.shader",VK_SHADER_STAGE_VERTEX_BIT},
			{"res/shaders/Frag.shader",VK_SHADER_STAGE_FRAGMENT_BIT},
		};

		pipeline.Create(Info);
	}
}

void Minecraft::UpdateCamera()
{
	float dt = Platform::Manager::GetDeltaTime();

	if (Platform::Input::IsKeyPressed(Key::W))
		Controller.MoveForward(dt);

	if (Platform::Input::IsKeyPressed(Key::S))
		Controller.MoveBackward(dt);
	
	if (Platform::Input::IsKeyPressed(Key::A))
		Controller.MoveLeft(dt);
	
	if (Platform::Input::IsKeyPressed(Key::D))
		Controller.MoveRight(dt);
	
	if (Platform::Input::IsKeyPressed(Key::Q))
		Controller.MoveDown(dt);
	
	if (Platform::Input::IsKeyPressed(Key::E))
		Controller.MoveUp(dt);

	static bool first = true;
	static float lastX, lastY, xOff, yOff, yaw, pitch;

	auto [x, y] = Platform::Input::GetMouseCursorPosition();

	if (first)
	{
		lastX = x;
		lastY = y;
		first = false;
	}

	xOff = x - lastX;
	yOff = y - lastY;

	lastX = x;
	lastY = y;	

	xOff *= 0.5f;
	yOff *= 0.5f;

	yaw += xOff;
	pitch += yOff;

	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	Controller.Rotate(yaw, pitch);
}
 