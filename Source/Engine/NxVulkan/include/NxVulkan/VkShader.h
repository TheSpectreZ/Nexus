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

	struct NEXUS_VULKAN_API ReflectionData
	{
		std::unordered_map<uint32_t, std::unordered_map<uint32_t,Nexus::ShaderResouceHeapLayoutBinding>> bindings;
		std::unordered_map<ShaderStage, VkPushConstantRange> ranges;
	};

	class NEXUS_VULKAN_API VulkanShader : public Shader
	{
		static const uint32_t maxHeapCountPerPool = 100;
		friend class VulkanCommandQueue;
	public:
		VulkanShader(const ShaderSpecification& specs);
		~VulkanShader() override;

		void AllocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void DeallocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void GetShaderResourceHeapLayoutBinding(ShaderResouceHeapLayoutBinding*& heap, uint32_t set,uint32_t binding) override;

		void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, uint32_t binding, Ref<Buffer> buffer) override;
		void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, ImageHandle texture) override;
		
		VkShaderModule GetModule(VkShaderStageFlagBits flag);
		VkPipelineLayout GetPipelineLayout() { return m_Layout; }
	private:
		std::unordered_map<ShaderStage,VkShaderModule> m_Modules;	
		std::unordered_map<uint32_t, SetResource> m_SetResource;
		ReflectionData m_ReflectionData;
		VkPipelineLayout m_Layout;
	};
}
