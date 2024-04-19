#include "NxVulkan/VkShader.h"
#include "NxVulkan/VkGraphicsDevice.h"

Nexus::VulkanShaderObject::VulkanShaderObject(const ShaderSpecification& specs, VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice)
{
	auto vkDevice = mDevice->getDevice();

	vk::ShaderModuleCreateInfo info{};
	info.codeSize = specs.spirv.size();
	info.pCode = (uint32_t*)specs.spirv.data();

	mModule = vkDevice.createShaderModule(info);
}

Nexus::VulkanShaderObject::~VulkanShaderObject()
{
	auto vkDevice = mDevice->getDevice();
	vkDevice.destroyShaderModule(mModule);
}
