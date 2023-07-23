#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"
#include "NxCore/Input.h"
#include "NxRenderEngine/Drawer.h"
#include "NxRenderEngine/Renderer.h"

Nexus::ForwardDrawer::ForwardDrawer(bool RenderToTexture)
{
	// Renderpass
	{
		std::vector<Nexus::RenderpassAttachmentDescription> attachments;
		{
			auto& color = attachments.emplace_back();
			color.type = Nexus::ImageType::Color;
			color.multiSampled = true;
			color.hdr = false;
			color.load = Nexus::ImageOperation::Clear;
			color.store = Nexus::ImageOperation::Store;
			color.initialLayout = Nexus::ImageLayout::Undefined;
			color.finalLayout = Nexus::ImageLayout::ColorAttachment;

			auto& depth = attachments.emplace_back();
			depth.type = Nexus::ImageType::Depth;
			depth.multiSampled = true;
			depth.hdr = false;
			depth.load = Nexus::ImageOperation::Clear;
			depth.store = Nexus::ImageOperation::DontCare;
			depth.initialLayout = Nexus::ImageLayout::Undefined;
			depth.finalLayout = Nexus::ImageLayout::DepthStencilAttachment;

			auto& resolve = attachments.emplace_back();
			resolve.type = Nexus::ImageType::Resolve;
			resolve.multiSampled = false;
			resolve.hdr = false;
			resolve.load = Nexus::ImageOperation::DontCare;
			resolve.store = Nexus::ImageOperation::Store;
			resolve.initialLayout = Nexus::ImageLayout::Undefined;

			if(RenderToTexture)
				resolve.finalLayout = Nexus::ImageLayout::ShaderReadOnly;
			else
				resolve.finalLayout = Nexus::ImageLayout::PresentSrc;
		}

		std::vector<Nexus::SubpassDescription> subpasses;
		{
			auto& subpass0 = subpasses.emplace_back();
			subpass0.ColorAttachments = { 0 };
			subpass0.DepthAttachment = 1;
			subpass0.ResolveAttachment = 2;
		}

		std::vector<Nexus::SubpassDependency> subpassDependencies;
		{
			auto& dep = subpassDependencies.emplace_back();
			dep.srcSubpass = Nexus::SubpassDependency::ExternalSubpass;
			dep.dstSubpass = 0;
			dep.srcStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput | Nexus::PipelineStageFlag::EarlyFragmentTest;
			dep.dstStageFlags = Nexus::PipelineStageFlag::ColorAttachmentOutput | Nexus::PipelineStageFlag::EarlyFragmentTest;
			dep.srcAccessFlags = Nexus::AccessFlag::None;
			dep.dstAccessFlags = Nexus::AccessFlag::ColorAttachmentWrite | Nexus::AccessFlag::DepthStencilAttachmentWrite;
		}

		RenderpassSpecification specs{};
		specs.attachments = &attachments;
		specs.subpasses = &subpasses;
		specs.dependencies = &subpassDependencies;

		m_pass = GraphicsInterface::CreateRenderpass(specs);
	}

	// Framebuffer
	{
		auto extent = Module::Renderer::Get()->GetSwapchain()->GetExtent();

		auto& a1 = m_fbSpecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::Color;
		a1.multisampled = true;
		a1.hdr = false;

		auto& a2 = m_fbSpecs.attachments.emplace_back();
		a2.Type = Nexus::FramebufferAttachmentType::DepthStencil;
		a2.multisampled = true;
		a2.hdr = false;

		auto& a3 = m_fbSpecs.attachments.emplace_back();

		if (RenderToTexture)
			a3.Type = Nexus::FramebufferAttachmentType::ShaderReadOnly_Color;
		else
			a3.Type = Nexus::FramebufferAttachmentType::PresentSrc;

		a3.multisampled = false;
		a3.hdr = false;

		m_fbSpecs.extent = extent;
		m_fbSpecs.renderpass = m_pass;

		m_fb = GraphicsInterface::CreateFramebuffer(m_fbSpecs);
	}

	// Pipeline
	{
		ShaderSpecification shaderSpecs = ShaderCompiler::CompileFromFile("Resources/Shaders/pbr.glsl");
		m_PbrShader = GraphicsInterface::CreateShader(shaderSpecs);

		shaderSpecs = ShaderCompiler::CompileFromFile("Resources/Shaders/skybox.glsl");
		m_SkyboxShader = GraphicsInterface::CreateShader(shaderSpecs);

		std::vector<VertexBindInfo> pipelineVertexBindInfo(1);
		{
			pipelineVertexBindInfo[0].binding = 0;
			pipelineVertexBindInfo[0].inputRate = VertexBindInfo::INPUT_RATE_VERTEX;

			// Depends on Vertex
			pipelineVertexBindInfo[0].stride = sizeof(Meshing::Vertex);
		}

		std::vector<VertexAttribInfo> pipelineVertexAttribInfo(5);
		{
			pipelineVertexAttribInfo[0].binding = 0;
			pipelineVertexAttribInfo[0].location = 0;
			pipelineVertexAttribInfo[0].offset = 0;
			pipelineVertexAttribInfo[0].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;

			pipelineVertexAttribInfo[1].binding = 0;
			pipelineVertexAttribInfo[1].location = 1;
			pipelineVertexAttribInfo[1].offset = sizeof(float) * 3;
			pipelineVertexAttribInfo[1].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;

			pipelineVertexAttribInfo[2].binding = 0;
			pipelineVertexAttribInfo[2].location = 2;
			pipelineVertexAttribInfo[2].offset = sizeof(float) * 6;
			pipelineVertexAttribInfo[2].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;

			pipelineVertexAttribInfo[3].binding = 0;
			pipelineVertexAttribInfo[3].location = 3;
			pipelineVertexAttribInfo[3].offset = sizeof(float) * 9;
			pipelineVertexAttribInfo[3].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;

			pipelineVertexAttribInfo[4].binding = 0;
			pipelineVertexAttribInfo[4].location = 4;
			pipelineVertexAttribInfo[4].offset = sizeof(float) * 12;
			pipelineVertexAttribInfo[4].format = VertexAttribInfo::ATTRIB_FORMAT_VEC2;
		}

		GraphicsPipelineSpecification pipelineSpecs{};
		pipelineSpecs.shader = m_PbrShader;
		pipelineSpecs.renderpass = m_pass;
		pipelineSpecs.subpass = 0;
		pipelineSpecs.multisampled = true;
		pipelineSpecs.rasterizerInfo.cullMode = CullMode::None;
		pipelineSpecs.rasterizerInfo.frontFace = FrontFaceType::Clockwise;
		pipelineSpecs.rasterizerInfo.lineWidth = 1.f;
		pipelineSpecs.rasterizerInfo.topology = TopologyType::TriangleList;
		pipelineSpecs.vertexBindInfo = pipelineVertexBindInfo;
		pipelineSpecs.vertexAttribInfo = pipelineVertexAttribInfo;

		pipelineSpecs.rasterizerInfo.polygonMode = PolygonMode::Line;
		m_PBR_LinePipeline = GraphicsInterface::CreatePipeline(pipelineSpecs);

		pipelineSpecs.rasterizerInfo.polygonMode = PolygonMode::Fill;
		m_PBR_FillPipeline = GraphicsInterface::CreatePipeline(pipelineSpecs);

		for (uint32_t i = 0; i < 4; i++)
			pipelineVertexAttribInfo.pop_back();

		pipelineSpecs.vertexAttribInfo = pipelineVertexAttribInfo;
		pipelineSpecs.shader = m_SkyboxShader;
		m_SkyboxPipeline = GraphicsInterface::CreatePipeline(pipelineSpecs);
	}

	// Screen
	{
		auto extent = Module::Renderer::Get()->GetSwapchain()->GetExtent();

		m_Viewport.x = 0.f;
		m_Viewport.y = 0.f;
		m_Viewport.width = (float)extent.width;
		m_Viewport.height = (float)extent.height;
		m_Viewport.minDepth = 0.f;
		m_Viewport.maxDepth = 1.f;

		m_Scissor.Extent = extent;
		m_Scissor.Offset = { 0,0 };
	}
}

// temporary
static uint32_t mode = 1;

void Nexus::ForwardDrawer::Draw(Ref<Scene> scene)
{
	if (Module::Input::Get()->IsKeyPressed(Key::Backspace, false))
		mode *= -1;

	UUID Id = scene->GetId();
	if (!m_RenderableScenes.contains(Id))
		m_RenderableScenes[Id] = CreateRef<RenderableScene>(m_PbrShader,m_SkyboxShader);

	m_RenderableScenes[Id]->Prepare(scene);

	auto commandQueue = Module::Renderer::Get()->GetCommandQueue();

	commandQueue->BeginRenderPass(m_pass, m_fb);

	commandQueue->BindPipeline(m_SkyboxPipeline);
	commandQueue->SetViewport(m_Viewport);
	commandQueue->SetScissor(m_Scissor);

	if (scene->GetRootEntity().environment.handle)
		m_RenderableScenes[Id]->DrawSkybox(commandQueue);

	commandQueue->BindPipeline(mode == 1 ? m_PBR_FillPipeline : m_PBR_LinePipeline);
	commandQueue->SetViewport(m_Viewport);
	commandQueue->SetScissor(m_Scissor);

	m_RenderableScenes[Id]->DrawScene(commandQueue,scene);

	commandQueue->EndRenderPass();
}

void Nexus::ForwardDrawer::OnWindowResize(Extent extent)
{
	m_fbSpecs.extent = extent;

	m_Viewport.x = 0.f;
	m_Viewport.y = 0.f;
	m_Viewport.width = (float)extent.width;
	m_Viewport.height = (float)extent.height;
	m_Viewport.minDepth = 0.f;
	m_Viewport.maxDepth = 1.f;

	m_Scissor.Extent = extent;
	m_Scissor.Offset = { 0,0 };

	m_fb.reset();
	m_fb = GraphicsInterface::CreateFramebuffer(m_fbSpecs);
}