#include "Graphics/Descriptor.h"
#include "Graphics/Engine.h"
#include "vkAssert.h"

void Nexus::Graphics::DescriptorPool::Create(std::vector<VkDescriptorPoolSize>* PoolSizes, uint32_t MaxSet)
{
	VkDescriptorPoolCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	Info.pNext = nullptr;
	Info.maxSets = MaxSet;
	Info.pPoolSizes = PoolSizes->data();
	Info.poolSizeCount = (uint32_t)PoolSizes->size();

	_VKR = vkCreateDescriptorPool(Engine::Get().GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle,VkDescriptorPool)
	NEXUS_LOG_INFO("Descriptor Pool Created")
}

void Nexus::Graphics::DescriptorPool::Destroy()
{
	vkDestroyDescriptorPool(Engine::Get().GetDevice(), m_handle, nullptr);
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

	_VKR = vkAllocateDescriptorSets(Engine::Get().GetDevice(), &Info, &set);
	CHECK_HANDLE(set, VkDescriptorSet)
	NEXUS_LOG_INFO("Descriptor Set Allocated")

	return set;
}

void Nexus::Graphics::Descriptor::BindWithBuffer(VkDescriptorSet set, VkBuffer buffer,VkDeviceSize size,uint32_t binding,uint32_t arrayElm)
{
	VkDescriptorBufferInfo b{};
	b.buffer = buffer;
	b.offset = 0;
	b.range = size;

	VkWriteDescriptorSet Info{};
	Info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	Info.pNext = nullptr;
	Info.descriptorCount = 1;
	Info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	Info.dstBinding = binding;
	Info.dstSet = set;
	Info.dstArrayElement = arrayElm;
	Info.pBufferInfo = &b;
	Info.pImageInfo = nullptr;
	Info.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(Engine::Get().GetDevice(), 1, &Info, 0, nullptr);
}

void Nexus::Graphics::Descriptor::BindWithCombinedImageSampler(VkDescriptorSet set, VkSampler sampler, VkImageView view,uint32_t binding, uint32_t arrayElm)
{
	VkDescriptorImageInfo i{};
	i.sampler = sampler;
	i.imageView = view;
	i.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet Info{};
	Info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	Info.pNext = nullptr;
	Info.descriptorCount = 1;
	Info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Info.dstBinding = binding;
	Info.dstSet = set;
	Info.dstArrayElement = arrayElm;
	Info.pBufferInfo = nullptr;
	Info.pImageInfo = &i;
	Info.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(Engine::Get().GetDevice(), 1, &Info, 0, nullptr);
}

void Nexus::Graphics::Descriptor::Bind(VkCommandBuffer cmdbuffer,VkPipelineLayout layout, uint32_t setIndex,VkDescriptorSet set)
{
	vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, setIndex, 1, &set, 0, nullptr);
}

void Nexus::Graphics::DescriptorLayout::Create(std::vector<VkDescriptorSetLayoutBinding>* bindings)
{
	VkDescriptorSetLayoutCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	Info.pNext = nullptr;
	Info.flags = 0;
	Info.pBindings = bindings->data();
	Info.bindingCount = (uint32_t)bindings->size();

	_VKR = vkCreateDescriptorSetLayout(Engine::Get().GetDevice(), &Info, nullptr, &m_handle);
	CHECK_HANDLE(m_handle, VkDescriptorSetLayout)
	NEXUS_LOG_INFO("Descriptor Set Layout Created")
}

void Nexus::Graphics::DescriptorLayout::Destroy()
{
	vkDestroyDescriptorSetLayout(Engine::Get().GetDevice(), m_handle, nullptr);
}
