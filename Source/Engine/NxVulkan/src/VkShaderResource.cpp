#include "NxVulkan/VkShaderResource.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkBuffer.h"

namespace Nexus
{
	static VkDescriptorType GetVulkanShaderResourceType(ShaderResourceType Type)
	{
		switch (Type)
		{
			case Nexus::ShaderResourceType::Uniform: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case Nexus::ShaderResourceType::SampledImage: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	static VkShaderStageFlags GetVulkanShaderStageFlag(ShaderStage Stage)
	{
		switch (Stage)
		{
		case Nexus::ShaderStage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case Nexus::ShaderStage::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		default:
			return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
	}
}

Nexus::VulkanShaderResourceHeapLayout::VulkanShaderResourceHeapLayout(const std::vector<ShaderResouceHeapLayoutBinding>& Bindings)
{
	m_Bindings = Bindings;

	std::vector<VkDescriptorSetLayoutBinding> bindings(m_Bindings.size());

	for (uint32_t i = 0; i < bindings.size(); i++)
	{
		bindings[i].binding = m_Bindings[i].bindPoint;
		bindings[i].descriptorCount = 1;
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = GetVulkanShaderStageFlag(m_Bindings[i].stage);
		bindings[i].descriptorType = GetVulkanShaderResourceType(m_Bindings[i].type);
	}

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.bindingCount = (uint32_t)bindings.size();
	createInfo.pBindings = bindings.data();

	_VKR = vkCreateDescriptorSetLayout(VulkanContext::Get()->GetDeviceRef()->Get(), &createInfo, nullptr, &m_layout);
	CHECK_HANDLE(m_layout, VkDescriptorSetLayout);
	NEXUS_LOG("Vulkan"," Descriptor Set Layout Created");
}

Nexus::VulkanShaderResourceHeapLayout::~VulkanShaderResourceHeapLayout()
{
	vkDestroyDescriptorSetLayout(VulkanContext::Get()->GetDeviceRef()->Get(), m_layout, nullptr);
	NEXUS_LOG("Vulkan"," Descriptor Set Layout Destroyed");
}

Nexus::VulkanShaderResourcePool::VulkanShaderResourcePool(Ref<ShaderResourceHeapLayout> layout, uint32_t maxResourceHeapCount)
{
	Ref<VulkanShaderResourceHeapLayout> vlay = DynamicPointerCast<VulkanShaderResourceHeapLayout>(layout);

	std::vector<VkDescriptorPoolSize> poolSizes(vlay->GetBindings().size());

	for (uint32_t i = 0; i < poolSizes.size(); i++)
	{
		poolSizes[i].type = GetVulkanShaderResourceType(vlay->GetBindings()[i].type);
		poolSizes[i].descriptorCount = maxResourceHeapCount;
	}

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.maxSets = maxResourceHeapCount;
	createInfo.flags = 0; // [To-Do] Make This Configurable
	createInfo.poolSizeCount = (uint32_t)poolSizes.size();
	createInfo.pPoolSizes = poolSizes.data();

	_VKR = vkCreateDescriptorPool(VulkanContext::Get()->GetDeviceRef()->Get(), &createInfo, nullptr, &m_pool);
	CHECK_HANDLE(m_pool, VkDescriptorSetPool);
	NEXUS_LOG("Vulkan"," Descriptor Set Pool Created");
}

Nexus::VulkanShaderResourcePool::~VulkanShaderResourcePool()
{
	vkDestroyDescriptorPool(VulkanContext::Get()->GetDeviceRef()->Get(), m_pool, nullptr);
	NEXUS_LOG("Vulkan"," Descriptor Pool Destroyed");
}