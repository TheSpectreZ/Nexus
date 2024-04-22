#pragma once
#include "NxGraphics/Buffer.h"

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

namespace Nexus
{
	class VulkanGPUBuffer : public GPUBuffer
	{
	public:
		VulkanGPUBuffer(const GPUBufferSpecification& specs, class VulkanGraphicsDevice* pDevice);
		~VulkanGPUBuffer();
	private:
		VulkanGraphicsDevice* mDevice;

		vk::Buffer mBuffer;
		VmaAllocation mAlloc;
	};
}
