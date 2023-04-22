#pragma once
#include "Renderer/ShaderResource.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanShaderResourceHeapLayout : public ShaderResourceHeapLayout
	{
	public:
		VulkanShaderResourceHeapLayout(const std::vector<ShaderResouceHeapLayoutBinding>& Bindings);
		~VulkanShaderResourceHeapLayout() override;

		VkDescriptorSetLayout& Get() { return m_layout; }
		std::vector<ShaderResouceHeapLayoutBinding>& GetBindings() { return m_Bindings; }
	private:
		VkDescriptorSetLayout m_layout;
		std::vector<ShaderResouceHeapLayoutBinding> m_Bindings;
	};

	class VulkanShaderResourcePool : public ShaderResourcePool
	{
	public:
		VulkanShaderResourcePool(Ref<ShaderResourceHeapLayout> layout,uint32_t maxResourceHeapCount);
		~VulkanShaderResourcePool() override;

		VkDescriptorPool Get() { return m_pool; }
		Ref<VulkanShaderResourceHeapLayout> GetLayout() { return m_layout; }
	private:
		VkDescriptorPool m_pool;
		Ref<VulkanShaderResourceHeapLayout> m_layout;
	};

	class VulkanShaderResourceHeap : public ShaderResourceHeap
	{
	public:
		VulkanShaderResourceHeap() = default;
		VulkanShaderResourceHeap(VkDescriptorSet set)
			:m_set(set)
		{}
		
		VkDescriptorSet& Get() { return m_set; }
	private:
		VkDescriptorSet m_set;
	};
}