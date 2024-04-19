#pragma once
#include "NxGraphics/Shader.h"

#ifdef NEXUS_SHADER_COMPILER_SHARED_BUILD
#define NEXUS_SHADER_COMPILER_API __declspec(dllexport)
#else
#define NEXUS_SHADER_COMPILER_API __declspec(dllimport)
#endif

namespace Nexus::ShaderCompiler
{
	extern "C"
	{
		NEXUS_SHADER_COMPILER_API bool CompileFromFile(const char* filepath, ShaderType type, ShaderSpecification* pOutSpecs);
	}
}
