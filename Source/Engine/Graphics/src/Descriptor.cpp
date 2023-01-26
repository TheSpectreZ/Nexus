#include "Graphics/Descriptor.h"
#include "Backend.h"
#include "vkAssert.h"

void Nexus::Graphics::DescriptorPool::Create(std::vector<VkDescriptorPoolSize>* PoolSizes, uint32_t MaxSet)
{
	VkDescriptorPoolCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	Info.pNext = nullptr;
	Info.maxSets = MaxSet;
	Info.pPoolSizes = PoolSizes->data();
	Info.poolSizeCount = (uint32_t)PoolSizes->size();

	_VKR = vkCreateDescriptorPool(Backend::GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle,VkDescriptorPool)
	NEXUS_LOG_INFO("Descriptor Pool Created")
}

void Nexus::Graphics::DescriptorPool::Destroy()
{
	vkDestroyDescriptorPool(Backend::GetDevice(), m_handle, nullptr);
}

VkDescriptorSet Nexus::Graphics::Descriptor::AllocateSet(DescriptorLayout* layout, DescriptorPool* pool)
{
	VkDescriptorSet set;

	VkDescriptorSetAllocateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	Info.pNext = nullptr;
	Info.descriptorPool = pool->Get();
	Info.pSetLayouts = &layout->Get();
	Info.descriptorSetCount = 1;

	_VKR = vkAllocateDescriptorSets(Backend::GetDevice(), &Info, &set);
	CHECK_HANDLE(set, VkDescriptorSet)
	NEXUS_LOG_INFO("Descriptor Set Allocated")

	return set;
}

void Nexus::Graphics::DescriptorLayout::Create(std::vector<VkDescriptorSetLayoutBinding>* bindings)
{
	VkDescriptorSetLayoutCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = 0;
	Info.pBindings = bindings->data();
	Info.bindingCount = (uint32_t)bindings->size();

	_VKR = vkCreateDescriptorSetLayout(Backend::GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkDescriptorSetLayout)
	NEXUS_LOG_INFO("Descriptor Set Layout Created")
}

void Nexus::Graphics::DescriptorLayout::Destroy()
{
	vkDestroyDescriptorSetLayout(Backend::GetDevice(), m_handle, nullptr);
}
