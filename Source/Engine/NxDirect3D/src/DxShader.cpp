#include "NxDirect3D/DxShader.h"
#include "NxGraphics/TypeImpls.h"

#include "shaderc/shaderc.hpp"

#include "spirv_cross/spirv_hlsl.hpp"

Nexus::DirectXShader::DirectXShader(const ShaderSpecification& specs)
{
	spirv_cross::CompilerHLSL::Options options;
	options.shader_model = 51;

	for (auto& [stage, spirv] : specs.spirv)
	{
		spirv_cross::CompilerHLSL compiler(spirv);
		compiler.set_hlsl_options(options);

		std::string str = compiler.compile();
		NEXUS_LOG("Shader", "STAGE: %s \n\n %s", GetShaderStageTypeStringName(stage).c_str(), str.c_str());
	}
}

Nexus::DirectXShader::~DirectXShader()
{
}

void Nexus::DirectXShader::AllocateShaderResourceHeap(ResourceHeapHandle handle)
{
}

void Nexus::DirectXShader::DeallocateShaderResourceHeap(ResourceHeapHandle handle)
{
}

void Nexus::DirectXShader::GetShaderResourceHeapLayoutBinding(ShaderResouceHeapLayoutBinding*& heap, uint32_t set, uint32_t binding)
{
}

void Nexus::DirectXShader::BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, uint32_t binding, Ref<Buffer> buffer)
{
}

void Nexus::DirectXShader::BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, ImageHandle texture)
{
}
