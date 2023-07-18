#pragma once
#include "NxCore/Base.h"
#include "Texture.h"
#include "Buffer.h"
#include "ShaderResource.h"

#include <vector>

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif // NEXUS_GRAPHICS_SHARED_BUILD

namespace Nexus
{
	typedef std::vector<uint32_t> SpirV;

	struct ResourceHeapHandle
	{
		uint64_t hashId;
		uint32_t set;
	};

	struct UniformBufferHandle
	{
		uint64_t hashId;
		uint32_t set;
		uint32_t binding;
	};

	struct ImageHandle
	{
		Ref<Texture> texture;
		Ref<Sampler> sampler;
		ShaderResourceType Type;
		uint32_t set;
		uint32_t binding;
		uint32_t miplevel = 0;
	};

	struct ShaderSpecification
	{
		std::unordered_map<ShaderStage, SpirV> spirv;
		std::string filepath;
	};

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		virtual void AllocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void DeallocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void GetShaderResourceHeapLayoutBinding(ShaderResouceHeapLayoutBinding*& layout,uint32_t set,uint32_t binding) = 0;

		virtual void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle,uint32_t binding, Ref<Buffer> buffer) = 0;
		virtual void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, ImageHandle texture) = 0;
	};

	namespace ShaderCompiler
	{
		Nexus::ShaderSpecification NEXUS_GRAPHICS_API CompileFromFile(const std::string& filepath);
	}
}