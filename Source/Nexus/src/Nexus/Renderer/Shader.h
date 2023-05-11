#pragma once
#include "RenderTypes.h"
#include "Buffer.h"
#include "ShaderResource.h"
#include "ResourcePool.h"
#include "Texture.h"
#include <filesystem>

typedef std::vector<uint32_t> SpirV;

namespace Nexus
{
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
		static Ref<Shader> Create(const std::string& Filepath);
		virtual void Destroy() = 0;

		Shader() = default;
		~Shader() = default;

		virtual void AllocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void DeallocateShaderResourceHeap(ResourceHeapHandle handle) = 0;
		virtual void BindShaderResourceHeap(ResourceHeapHandle handle) = 0;

		virtual void AllocateUniformBuffer(UniformBufferHandle handle) = 0;
		virtual void DeallocateUniformBuffer(UniformBufferHandle handle) = 0;
		
		virtual void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, UniformBufferHandle bufferHandle) = 0;
		virtual void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, CombinedImageSamplerHandle texture) = 0;
		
		virtual void SetUniformData(UniformBufferHandle handle, void* data) = 0;
	};

	class ShaderLib
	{
		friend class VulkanShader;

		static ShaderLib* s_Instance;
	public:
		static void Initialize();
		static void Terminate();

		static Ref<Shader> Get(const std::string& shaderPath);
	private:
		ResourcePool m_ResourcePool;
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
