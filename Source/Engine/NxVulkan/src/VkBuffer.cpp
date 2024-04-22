#include "NxVulkan/VkBuffer.h"
#include "NxVulkan/VkGraphicsDevice.h"

Nexus::VulkanGPUBuffer::VulkanGPUBuffer(const GPUBufferSpecification& specs, VulkanGraphicsDevice* pDevice)
	:mDevice(pDevice)
{
	auto vkDevice = mDevice->getDevice();
	auto vkAllocator = mDevice->getAllocator();

	vk::BufferCreateInfo bInfo{};
}

Nexus::VulkanGPUBuffer::~VulkanGPUBuffer()
{
}
