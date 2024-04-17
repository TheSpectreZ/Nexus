#pragma once
#include <string>
#include <vector>

#ifdef NEXUS_SHADER_COMPILER_SHARED_BUILD
#define NEXUS_SHADER_COMPILER_API __declspec(dllexport)
#else
#define NEXUS_SHADER_COMPILER_API __declspec(dllimport)
#endif

namespace Nexus::ShaderCompiler
{
	enum class ShaderType
	{
		Vertex		= 0,
		Fragment	= 1,
		Compute		= 2
	};

	struct ShaderBlob
	{
		std::vector<uint32_t> spirv;
		ShaderType type;
	};
	
	extern "C"
	{
		NEXUS_SHADER_COMPILER_API bool CompileFromFile(const std::string& filepath, ShaderType type, ShaderBlob* pInBlob);
	}
}
