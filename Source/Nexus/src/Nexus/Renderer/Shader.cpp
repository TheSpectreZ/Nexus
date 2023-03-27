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

	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		NEXUS_LOG_ERROR("{0}", result.GetErrorMessage().c_str());
		NEXUS_ASSERT("Shader Compilation Error: {0}", file);
	}

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

	SpirV vertexShader = CompileSource(vertexSrc, shaderc_glsl_vertex_shader, std::string(Filepath.c_str() + std::string(" : Vertex Shader")).c_str());
	SpirV fragmentShader = CompileSource(fragmentSrc, shaderc_glsl_fragment_shader, std::string(Filepath.c_str() + std::string(" : Fragment Shader")).c_str());

	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanShader>(vertexShader, fragmentShader, Filepath.c_str());
		case RenderAPIType::NONE: return nullptr;
		default: return nullptr;
	}
}

Nexus::ShaderLib* Nexus::ShaderLib::s_Instance = nullptr;

void Nexus::ShaderLib::Initialize()
{
	s_Instance = new ShaderLib();
}

void Nexus::ShaderLib::Terminate()
{
	std::cout << std::endl;
	for (auto& [k, v] : s_Instance->m_Shaders)
	{
		v->Destroy();
		NEXUS_LOG_DEBUG("Shader Removed: {0}", k);
	}
	std::cout << std::endl;

	delete s_Instance;
}

Nexus::Ref<Nexus::Shader> Nexus::ShaderLib::Get(const std::string& shaderPath)
{
	if (s_Instance->m_Shaders.contains(shaderPath))
	{
		NEXUS_LOG_DEBUG("Shader Found: {0}", shaderPath);
		return s_Instance->m_Shaders[shaderPath];
	}

	s_Instance->m_Shaders[shaderPath] = Shader::Create(shaderPath);
	return s_Instance->m_Shaders[shaderPath];
}
