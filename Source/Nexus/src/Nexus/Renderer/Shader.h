#pragma once
#include "RenderTypes.h"
#include "Buffer.h"
#include "ShaderResource.h"
#include "ResourcePool.h"
#include <filesystem>

typedef std::vector<uint32_t> SpirV;

namespace Nexus
{
	class Shader
	{
	public:		
		static Ref<Shader> Create(const std::string& Filepath);
		virtual void Destroy() = 0;

		Shader() = default;
		~Shader() = default;

		virtual void AllocateShaderResourceHeap(uint64_t hashId,uint32_t set) = 0;
		virtual void DeallocateShaderResourceHeap(uint64_t hashId, uint32_t set) = 0;
		virtual void BindShaderResourceHeap(uint64_t hashId, uint32_t set) = 0;

		virtual void AllocateUniformBuffer(uint64_t hashId, uint32_t set, uint32_t binding) = 0;
		virtual void DeallocateUniformBuffer(uint64_t hashId) = 0;
		virtual void BindUniformWithResourceHeap(uint64_t uniformId, uint64_t heapId, uint32_t set, uint32_t binding) = 0;
		virtual void SetUniformData(uint64_t uniformId,void* data) = 0;
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
