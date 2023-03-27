#pragma once
#include "RenderTypes.h"

namespace Nexus
{	
	/// 
	/// Shader Resource Heap Layout (VkDescriptorSetLayout)
	/// 

	struct ShaderResouceHeapLayoutBinding
	{
		uint32_t bindPoint;
		ShaderResourceType type;
		ShaderStage stage;
	};

	class ShaderResourceHeapLayout
	{
	public:
		ShaderResourceHeapLayout() = default;
		virtual ~ShaderResourceHeapLayout() = default;

		static Ref<ShaderResourceHeapLayout> Create(const std::vector<ShaderResouceHeapLayoutBinding>& Bindings);
	};

	/// 
	/// Shader Resource Heap (VkDescriptorSet)
	/// 

	class ShaderResourceHeap
	{
	public:
		ShaderResourceHeap() = default;
		virtual ~ShaderResourceHeap() = default;
	};

	/// 
	/// Shader Resource Pool (VkDescriptorPool)
	/// 

	class ShaderResourcePool
	{
	public:
		ShaderResourcePool() = default;
		virtual ~ShaderResourcePool() = default;

		static Ref<ShaderResourcePool> Create(Ref<ShaderResourceHeapLayout> layout, uint32_t maxResourceHeapCount);

		virtual void AllocateShaderResourceHeaps(ShaderResourceHeap* pHeaps, uint32_t count) = 0;
	};
}