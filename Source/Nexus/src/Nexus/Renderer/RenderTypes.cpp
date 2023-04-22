#include "nxpch.h"
#include "RenderTypes.h"

std::string Nexus::GetShaderStageTypeStringName(ShaderStage stage)
{
	switch (stage)
	{
	case Nexus::ShaderStage::Vertex:
		return "Vertex Shader";
	case Nexus::ShaderStage::Fragment:
		return "Fragment Shader";
	default:
		return std::string();
	}
}

std::string Nexus::GetBufferTypeStringName(BufferType Type)
{
	switch (Type)
	{
	case Nexus::BufferType::Vertex:
		return "Vertex Buffer";
	case Nexus::BufferType::Index:
		return "Index Buffer";
	default:
		return std::string();
	}
}

std::string Nexus::GetShaderResourceTypeStringName(ShaderResourceType Type)
{
	switch (Type)
	{
	case Nexus::ShaderResourceType::Uniform:
		return "Uniform Buffer";
	case Nexus::ShaderResourceType::SampledImage:
		return "Sampled Image";
	default:
		return std::string();
	}
}