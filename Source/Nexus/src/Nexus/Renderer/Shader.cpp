#include "nxpch.h"
#include "Shader.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkShader.h"

#include "shaderc/shaderc.hpp"

static SpirV CompileSource(const std::string& src, shaderc_shader_kind kind,const char* file)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

 	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(src, kind, file, options);
	NEXUS_ASSERT((result.GetCompilationStatus() != shaderc_compilation_status_success), result.GetErrorMessage().c_str());

	return std::vector<uint32_t>(result.cbegin(), result.cend());
}

static void ParseFile(const std::string& FilePath, std::string* vertexSrc, std::string* fragmentSrc)
{
	enum ShaderType
	{
		ShNone = -1, ShVertex = 0, ShFragment = 1
	} Type = ShNone;

	std::ifstream stream(FilePath, std::ios::in);
	NEXUS_ASSERT((!stream), "Failed to Open Shader");

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

Nexus::Ref<Nexus::Shader> Nexus::Shader::Create(const std::string& Filepath)
{
	std::string vertexSrc, fragmentSrc;
	ParseFile(Filepath, &vertexSrc, &fragmentSrc);

	SpirV vertexShader = CompileSource(vertexSrc, shaderc_glsl_vertex_shader,Filepath.c_str());
	SpirV fragmentShader = CompileSource(fragmentSrc, shaderc_glsl_fragment_shader,Filepath.c_str());

	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanShader>(vertexShader, fragmentShader,Filepath.c_str());
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}
