#include "Minecraft.h"
#include "Graphics/Presenter.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Platform/Input.h"
#include "Platform/Manager.h"

#include "DebugUtils/Logger.h"


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

	
	// Uniforms
	{
		worldbuffer.Create(sizeof(Camera));
		Descriptor::BindWithBuffer(descriptorSet, worldbuffer.Get(), sizeof(Camera), 0, 0);

		instancebuffer.Create(sizeof(InstanceData));
		Descriptor::BindWithBuffer(descriptorSet, instancebuffer.Get(), sizeof(InstanceData), 1, 0);
	}

	// Screen
	{
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.maxDepth = 1.f;
		viewport.minDepth = 0.f;
		viewport.height = (float)Presenter::GetImageExtent().height;
		viewport.width = (float)Presenter::GetImageExtent().width;
		
		scissor.extent = Presenter::GetImageExtent();
		scissor.offset = { 0,0 };
	}

	m_world.Create();

	texture.Create("res/textures/Blocks.jpg");

	NEXUS_LOG_DEBUG("{0}x{1}", texture.GetExtent().width, texture.GetExtent().height);

	sampler.Create(VK_FILTER_NEAREST, VK_FILTER_LINEAR);

	Descriptor::BindWithCombinedImageSampler(descriptorSet, sampler.Get(), texture.Get(), 2, 0);
}

int pipeline = 0;

void Minecraft::OnUpdate()
{
	UpdateCamera();
	worldbuffer.Update(&m_world.GetPlayer().GetRawCameraData());

	instancedata.cameraPos = m_world.GetPlayer().GetPosition();
	instancebuffer.Update(&instancedata);

	using namespace Platform;

	if (Input::IsKeyPressed(Key::P))
		pipeline = 0;
	if (Input::IsKeyPressed(Key::O))
		pipeline = 1;

	m_world.Update();
}

void Minecraft::OnRender()
{
	VkCommandBuffer cmdBuffer = Presenter::GetCommandBuffer();

	Presenter::BeginRenderpass(cmdBuffer, renderpass, Framebuffers[Presenter::GetFrameIndex()], clearValue);
	
	Descriptor::Bind(cmdBuffer, pipelineLayout.Get(), 0, descriptorSet);
	
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[pipeline].Get());

	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	m_world.Render(cmdBuffer);

	Presenter::EndRenderpass(cmdBuffer);
}

void Minecraft::OnDetach()
{
	texture.Destroy();
	sampler.Destroy();

	m_world.Destroy();

	worldbuffer.Destroy();
	instancebuffer.Destroy();

	pipelines[0].Destroy();
	pipelines[1].Destroy();

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

	m_world.GetPlayer().OnCallback();

	// Screen
	{
		auto extent = Presenter::GetImageExtent();
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.maxDepth = 1.f;
		viewport.minDepth = 0.f;
		viewport.height = (float)extent.height;
		viewport.width = (float)extent.width;

		scissor.extent = extent;
		scissor.offset = { 0,0 };
	}
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
			color.flags = 0;
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
			depth.flags = 0;
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
			resolve.flags = 0;
		}

		auto& Dependency = Info.dependecies.emplace_back();
		{
			Dependency.dependencyFlags = 0;
			Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			Dependency.dstSubpass = 0;
			Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			Dependency.srcAccessMask = 0;
			Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		VkAttachmentReference cRef{}, dRef{}, rRef{};
		cRef.attachment = 0;
		cRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		dRef.attachment = 1;
		dRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		rRef.attachment = 2;
		rRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto& SubpassDescription = Info.subpasses.emplace_back();
		{
			SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			SubpassDescription.colorAttachmentCount = 1;
			SubpassDescription.pColorAttachments = &cRef;
			SubpassDescription.pDepthStencilAttachment = &dRef;
			SubpassDescription.pResolveAttachments = &rRef;
			
			SubpassDescription.inputAttachmentCount = 0;
			SubpassDescription.pInputAttachments = nullptr;
			SubpassDescription.preserveAttachmentCount = 0;
			SubpassDescription.pPreserveAttachments = nullptr;
			SubpassDescription.flags = 0;
		}
		renderpass.Create(Info);
	}
}

void Minecraft::CreateDescriptors()
{
	// Pool
	{
		std::vector<VkDescriptorPoolSize> sizes = { 
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,2},
		};

		descriptorPool.Create(&sizes, 1);
	}

	// Layout
	{
		std::vector<VkDescriptorSetLayoutBinding> layouts =
		{
			{0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1,VK_SHADER_STAGE_VERTEX_BIT ,nullptr},
			{1,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1,VK_SHADER_STAGE_FRAGMENT_BIT ,nullptr},
			{2,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,VK_SHADER_STAGE_FRAGMENT_BIT ,nullptr},
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
		
		Info.cullMode = VK_CULL_MODE_NONE;
		Info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		Info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		Info.samples = Presenter::GetImageMaxSamples();

		Info.renderPass = renderpass.Get();
		Info.subPassIndex = 0;

		Info.dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		Info.viewportCount = 1;
		Info.pViewports = nullptr;
		Info.scissorCount = 1;
		Info.pScissors = nullptr;

		Info.vertexBindings = { {0,sizeof(Vertex),VK_VERTEX_INPUT_RATE_VERTEX} };

		Info.vertexAttributes =
		{
			{0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Vertex::position) },
			{1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Vertex::TextureInfo) },
		};

		Info.ShaderPaths =
		{
			{"res/shaders/Vert.shader",VK_SHADER_STAGE_VERTEX_BIT},
			{"res/shaders/Frag.shader",VK_SHADER_STAGE_FRAGMENT_BIT},
		};

		Info.polygonMode = VK_POLYGON_MODE_FILL;
		pipelines[0].Create(Info);

		Info.polygonMode = VK_POLYGON_MODE_LINE;
		pipelines[1].Create(Info);
	}
}

void Minecraft::UpdateCamera()
{
	
}