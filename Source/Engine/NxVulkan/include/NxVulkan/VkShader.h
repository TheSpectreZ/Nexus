#pragma once
#include "NxGraphics/Shader.h"
#include "VkShaderResource.h"
#include "VkAssert.h"

#ifdef NEXUS_VULKAN_SHARED_BUILD
#define NEXUS_VULKAN_API __declspec(dllexport)
#else
#define NEXUS_VULKAN_API __declspec(dllimport)
#endif

namespace Nexus
{
	struct NEXUS_VULKAN_API SetResource
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

	struct VertexInputReflectData
	{

	};

	struct NEXUS_VULKAN_API ReflectionData
	{
		std::unordered_map<uint32_t, std::vector<Nexus::ShaderResouceHeapLayoutBinding>> bindings;
	};

	class NEXUS_VULKAN_API VulkanShader : public Shader
	{
		static const uint32_t maxHeapCountPerPool = 100;
	public:
		VulkanShader(const ShaderSpecification& specs);
		~VulkanShader() override;

		void AllocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void DeallocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void BindShaderResourceHeap(ResourceHeapHandle handle) override;

		void AllocateUniformBuffer(UniformBufferHandle handle) override;
		void DeallocateUniformBuffer(UniformBufferHandle handle) override;
		void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, UniformBufferHandle bufferHandle) override;
		void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, CombinedImageSamplerHandle texture) override;
		void SetUniformData(UniformBufferHandle handle, void* data) override;

		VkShaderModule GetModule(VkShaderStageFlagBits flag);
		VkPipelineLayout GetPipelineLayout() { return m_Layout; }
	private:
		VkShaderModule m_VertexModule, m_FragmentModule;	
		std::unordered_map<uint32_t, SetResource> m_SetResource;
		ReflectionData m_ReflectionData;
		VkPipelineLayout m_Layout;
	};
}
