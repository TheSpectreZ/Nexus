#pragma once
#include "Renderer/Shader.h"
#include "Renderer/ResourcePool.h"

#include "VkShaderResource.h"
#include "VkAssert.h"

namespace Nexus
{
	struct SetResource
	{
		Ref<VulkanShaderResourceHeapLayout> HeapLayout;
		std::vector<Ref<VulkanShaderResourcePool>> HeapPools;
		std::unordered_map<uint64_t, VulkanShaderResourceHeap> Heaps;
		std::unordered_map<uint64_t, bool> GarbageHeaps;

		~SetResource()
		{
			HeapPools.clear();
			Heaps.clear();
		}
	};

	struct ReflectionData
	{
		std::unordered_map<uint32_t, std::vector<Nexus::ShaderResouceHeapLayoutBinding>> bindings;
	};

	class VulkanShader : public Shader
	{
		static const uint32_t maxHeapCountPerPool = 100;
	public:
		VulkanShader(SpirV& vertexData, SpirV& fragmentData, const char* Filepath);
		void Destroy() override;

		void AllocateShaderResourceHeap(uint64_t hashId, uint32_t set) override;
		void DeallocateShaderResourceHeap(uint64_t hashId, uint32_t set) override;
		void BindShaderResourceHeap(uint64_t hashId, uint32_t set) override;

		void AllocateUniformBuffer(uint64_t hashId, uint32_t set, uint32_t binding) override;
		void DeallocateUniformBuffer(uint64_t hashId) override;
		void BindUniformWithResourceHeap(uint64_t uniformId, uint64_t heapId, uint32_t set, uint32_t binding) override;
		void SetUniformData(uint64_t uniformId, void* data) override;

		VkShaderModule GetModule(VkShaderStageFlagBits flag);
		VkPipelineLayout GetPipelineLayout() { return m_Layout; }
	private:
		VkShaderModule m_VertexModule, m_FragmentModule;	
		std::unordered_map<uint32_t, SetResource> m_SetResource;
		ReflectionData m_ReflectionData;
		VkPipelineLayout m_Layout;
	};
}
