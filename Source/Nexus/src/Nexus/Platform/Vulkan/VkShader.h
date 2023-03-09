#pragma once
#include "Renderer/Shader.h"
#include "VkAssert.h"

namespace Nexus
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(SpirV& vertexData, SpirV& fragmentData,const char* Filepath);
		~VulkanShader() override;

		VkShaderModule GetModule(VkShaderStageFlagBits flag);
	private:
		VkShaderModule m_VertexModule, m_FragmentModule;
	};
}
