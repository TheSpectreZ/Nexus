#pragma once
#include "NxGraphics/Shader.h"
#include "vulkan/vulkan.hpp"

namespace Nexus
{
	class VulkanShaderObject : public ShaderObject
	{
	public:
		VulkanShaderObject(const ShaderSpecification& specs, class VulkanGraphicsDevice* pDevice);
		~VulkanShaderObject() override;
	private:
		VulkanGraphicsDevice* mDevice;
		vk::ShaderModule mModule;
	};
}
