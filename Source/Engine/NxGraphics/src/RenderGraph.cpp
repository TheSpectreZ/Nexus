#include "NxGraphics/RenderGraph.h"

Nexus::RenderTargetSpecification& Nexus::RenderTargetSpecification::set(Extent extent)
{
	this->extent = extent;
	return *this;
}

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

Nexus::RenderTargetSpecification& Nexus::RenderGraphPassSpecification::setOutput(const std::string& name)
{
	m_Outputs.push_back(name);
	return m_Parent->m_RenderTargets[name];
}

Nexus::RenderGraphPassSpecification& Nexus::RenderGraph::AddRenderGraphPass(const std::string& name)
{
	m_GPUpasses[name] = RenderGraphPassSpecification(this);
	return m_GPUpasses[name];
}