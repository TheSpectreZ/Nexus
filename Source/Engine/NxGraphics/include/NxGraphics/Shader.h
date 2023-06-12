#pragma once
#include "NxCore/Base.h"
#include "Texture.h"
#include <vector>

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

	struct CombinedImageSamplerHandle
	{
		Ref<Texture> texture;
		Ref<Sampler> sampler;
		uint32_t set;
		uint32_t binding;
	};

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		virtual void AllocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void DeallocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void BindShaderResourceHeap(ResourceHeapHandle handle) = 0;

		virtual void AllocateUniformBuffer(UniformBufferHandle handle) = 0;
		virtual void DeallocateUniformBuffer(UniformBufferHandle handle) = 0;

		virtual void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, UniformBufferHandle bufferHandle) = 0;
		virtual void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, CombinedImageSamplerHandle texture) = 0;

		virtual void SetUniformData(UniformBufferHandle handle, void* data) = 0;
	};

	
}