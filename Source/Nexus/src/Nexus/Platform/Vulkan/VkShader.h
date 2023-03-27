#pragma once
#include "Renderer/Shader.h"
#include "VkShaderResource.h"
#include "VkAssert.h"

namespace Nexus
{
	struct SetResource
	{
		Ref<VulkanShaderResourceHeapLayout> HeapLayout;
		std::vector<Ref<VulkanShaderResourcePool>> HeapPools;
		std::unordered_map<uint32_t, VulkanShaderResourceHeap> Heaps;

		~SetResource()
		{
			HeapPools.clear();
			Heaps.clear();
		}
	};

	class VulkanShader : public Shader
	{
		static const uint32_t maxHeapCountPerPool = 100;
	public:
		VulkanShader(SpirV& vertexData, SpirV& fragmentData, const char* Filepath);
		void Destroy() override;

		VkShaderModule GetModule(VkShaderStageFlagBits flag);
	private:
		VkShaderModule m_VertexModule, m_FragmentModule;	
		std::unordered_map<uint32_t, SetResource> m_SetResource;
	};
}
