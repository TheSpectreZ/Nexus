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
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(src, kind, file, options);

		NEXUS_BREAK_ASSERT((result.GetCompilationStatus() != shaderc_compilation_status_success),
			"Shader Compilation Error: %s - %s", file, result.GetErrorMessage().c_str());
		
		return std::vector<uint32_t>(result.cbegin(), result.cend());
	}

	static void ParseFile(const std::string& FilePath, std::string* vertexSrc, std::string* fragmentSrc)
	{
		enum ShaderType
		{
			ShNone = -1, ShVertex = 0, ShFragment = 1
		} Type = ShNone;

		std::ifstream stream(FilePath, std::ios::in);
		NEXUS_BREAK_ASSERT((!stream), "Failed to Open Shader");

		std::stringstream ss[2]; std::string line;

		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("VERTEX") != std::string::npos)
					Type = ShVertex;
				else if (line.find("FRAGMENT") != std::string::npos)
					Type = ShFragment;
			}
			else
			{
				ss[(uint32_t)Type] << line << "\n";
			}
		}

		*vertexSrc = ss[0].str();
		*fragmentSrc = ss[1].str();
	}
}

Nexus::ShaderSpecification Nexus::ShaderCompiler::CompileFromFile(const std::string& filepath)
{
	std::string vs, fs;
	ParseFile(filepath, &vs, &fs);

	ShaderSpecification specs;
	specs.filepath = filepath;
	specs.vertexData = CompileSource(vs, shaderc_vertex_shader, specs.filepath.c_str());
	specs.fragmentData = CompileSource(fs, shaderc_fragment_shader, specs.filepath.c_str());

	return specs;
}
