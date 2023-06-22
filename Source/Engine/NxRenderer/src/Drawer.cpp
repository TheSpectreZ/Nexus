#include "NxRenderer/Drawer.h"
#include "NxRenderer/Renderer.h"

Nexus::ForwardDrawer::ForwardDrawer()
{
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
			color.finalLayout = Nexus::ImageLayout::ColorAttachment;

			auto& depth = attachments.emplace_back();
			depth.type = Nexus::ImageType::Depth;
			depth.multiSampled =false;
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
			resolve.finalLayout = Nexus::ImageLayout::ShaderReadOnly;
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

	// Pipeline
	{
		ShaderSpecification shaderSpecs = ShaderCompiler::CompileFromFile("Resources/Shaders/pbr.shader");
		m_shader = GraphicsInterface::CreateShader(shaderSpecs);

		PipelineSpecification pipelineSpecs{};
		pipelineSpecs.shader = m_shader;
		pipelineSpecs.renderpass = m_pass;
		pipelineSpecs.subpass = 0;
		pipelineSpecs.multisampled = false;
		pipelineSpecs.rasterizerInfo.cullMode = CullMode::Back;
		pipelineSpecs.rasterizerInfo.frontFace = FrontFaceType::Clockwise;
		pipelineSpecs.rasterizerInfo.lineWidth = 1.f;
		pipelineSpecs.rasterizerInfo.topology = TopologyType::TriangleList;
		pipelineSpecs.rasterizerInfo.polygonMode = PolygonMode::Fill;

		m_pipeline = GraphicsInterface::CreatePipeline(pipelineSpecs);
	}

	// Framebuffer
	{
		auto extent = Module::Renderer::Get()->GetSwapchain()->GetExtent();

		auto& a1 = m_fbSpecs.attachments.emplace_back();
		a1.Type = Nexus::FramebufferAttachmentType::Color;
		a1.multisampled = false;
		a1.hdr = false;

		auto& a2 = m_fbSpecs.attachments.emplace_back();
		a2.Type = Nexus::FramebufferAttachmentType::DepthStencil;
		a2.multisampled = false;
		a2.hdr = false;

		auto& a3 = m_fbSpecs.attachments.emplace_back();
		a3.Type = Nexus::FramebufferAttachmentType::ShaderReadOnly_Color;
		a3.multisampled = false;
		a3.hdr = false;

		m_fbSpecs.extent = extent;
		m_fbSpecs.renderpass = m_pass;

		m_fb = GraphicsInterface::CreateFramebuffer(m_fbSpecs);
	}
}

Nexus::ForwardDrawer::~ForwardDrawer()
{
	m_pipeline.reset();
	m_shader.reset();
	m_fb.reset();
	m_pass.reset();
}

void Nexus::ForwardDrawer::Draw(Ref<Scene> scene)
{
}

void Nexus::ForwardDrawer::OnWindowResize(Extent extent)
{
	m_fbSpecs.extent = extent;

	m_fb.reset();
	m_fb = GraphicsInterface::CreateFramebuffer(m_fbSpecs);
}
