#include "NxGraphics/TypeImpls.h"

std::string Nexus::GetShaderStageTypeStringName(ShaderStage stage)
{
	std::string name;

	if (Nexus::IsFlagSet(stage, Nexus::ShaderStage::Vertex))
		name = "[Vertex] ";
	if (Nexus::IsFlagSet(stage, Nexus::ShaderStage::Fragment))
		name += "[Fragment]";
	if (Nexus::IsFlagSet(stage, Nexus::ShaderStage::Compute))
		name += "[Compute]";
	
	return name;
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
	case Nexus::ShaderResourceType::SeperateImage:
		return "Seperate Image";
	case Nexus::ShaderResourceType::StorageImage:
		return "Storage Image";
	default:
		return "None";
	}
}