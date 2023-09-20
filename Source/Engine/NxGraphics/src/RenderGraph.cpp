#include "NxGraphics/RenderGraph.h"

//Nexus::RenderTargetSpecification& Nexus::RenderTargetSpecification::set(Extent extent)
//{
//	this->extent = extent;
//	return *this;
//}

Nexus::RenderTargetSpecification& Nexus::RenderTargetSpecification::set(TextureFormat format)
{
	this->format = format;
	return *this;
}

Nexus::RenderTargetSpecification& Nexus::RenderTargetSpecification::set(RenderTargetUsage usage)
{
	this->usage = usage;
	return *this;
}

Nexus::RenderTargetSpecification& Nexus::RenderTargetSpecification::enableMSAA(bool multiSampled)
{
	this->multiSampled = multiSampled;
	return *this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::set(RenderPipelineTopology topology)
{
	this->topology = topology;
	return *this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::set(RenderPipelinePolygonMode polygon)
{
	this->polygon = polygon;
	return *this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::set(RenderPipelineCullMode cullMode)
{
	this->cull = cullMode;
	return	*this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::set(RenderPiplineFrontFaceType face)
{
	this->frontface = face;
	return *this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::set(Ref<Shader> shader)
{
	this->shader = shader;
	return *this;
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::GraphicsRenderPipelineSpecification::enableMSAA(bool multiSampled)
{
	this->multiSampled = multiSampled;
	return *this;
}

VertexBindInfo& Nexus::GraphicsRenderPipelineSpecification::addVertexBinding()
{
	return bindInfo.emplace_back();
}

VertexAttribInfo& Nexus::GraphicsRenderPipelineSpecification::addVertexAttribute()
{
	return attribInfo.emplace_back();
}

Nexus::RenderTargetSpecification& Nexus::RenderGraphPassSpecification::addOutput(const std::string& name)
{
	m_Outputs.push_back(name);
	return m_Parent->m_RenderTargets[name];
}

Nexus::GraphicsRenderPipelineSpecification& Nexus::RenderGraphPassSpecification::addGraphicsPipeline(const std::string& name)
{
	m_GraphicsPipelines.push_back(name);
	return m_Parent->m_GraphicsRenderPipelines[name];
}

Nexus::RenderGraphPassSpecification& Nexus::RenderGraphPassSpecification::setRenderTargetExtent(Extent extent)
{
	m_Extent = extent;
	return *this;
}

Nexus::RenderGraphPassSpecification& Nexus::RenderGraph::AddRenderGraphPass(const std::string& name)
{
	m_GPUpasses[name] = RenderGraphPassSpecification(this);
	return m_GPUpasses[name];
}

