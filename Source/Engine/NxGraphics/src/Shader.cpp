#include "NxGraphics/Shader.h"
#include "shaderc/shaderc.hpp"
#include "NxCore/Assertion.h"

#include <fstream>
#include <sstream>

namespace Nexus
{
	static SpirV CompileSource(const std::string& src, shaderc_shader_kind kind, const char* file)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
	
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(src, kind, file, options);
		
		NEXUS_BREAK_ASSERT((result.GetCompilationStatus() != shaderc_compilation_status_success),
			"Shader Compilation Error: %s - %s", file, result.GetErrorMessage().c_str());

		return std::vector<uint32_t>(result.cbegin(), result.cend());
	}

	shaderc_shader_kind GetShaderCKind(Nexus::ShaderStage stage)
	{
		switch (stage)
		{
		case Nexus::ShaderStage::Vertex:
			return shaderc_vertex_shader;
		case Nexus::ShaderStage::Fragment:
			return shaderc_fragment_shader;
		case Nexus::ShaderStage::Compute:
			return shaderc_compute_shader;
		default:
			return shaderc_shader_kind();
		}
	}
}

Nexus::ShaderSpecification Nexus::ShaderCompiler::CompileFromFile(const std::string& filepath)
{
	std::ifstream Infile(filepath, std::ios::in);
	NEXUS_BREAK_ASSERT((!Infile), "Failed to Open Shader");

	std::string line;	
	std::unordered_map<ShaderStage, std::stringstream> src;

	ShaderStage mapStage = ShaderStage::None;
	while (getline(Infile, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("VERTEX") != std::string::npos)
				mapStage = ShaderStage::Vertex;
			else if (line.find("FRAGMENT") != std::string::npos)
				mapStage = ShaderStage::Fragment;
			else if (line.find("COMPUTE") != std::string::npos)
				mapStage = ShaderStage::Compute;
		}
		else
		{
			src[mapStage] << line << "\n";
		}
	}

	ShaderSpecification specs;
	specs.filepath = filepath;
	
	for (auto& [k, ss] : src)
	{
		specs.spirv[k] = CompileSource(ss.str(), GetShaderCKind(k), specs.filepath.c_str());
	}
	
	return specs;
}
