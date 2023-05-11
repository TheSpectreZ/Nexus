#include "nxpch.h"
#include "VkContext.h"
#include "VkCommandQueue.h"
#include "VulkanEditorViewport.h"
#include "backends/imgui_impl_vulkan.cpp"

Nexus::VulkanEditorViewport::VulkanEditorViewport()
{
    // Sampler
    {
        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_NEAREST;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties Props;
        vkGetPhysicalDeviceProperties(VulkanContext::Get()->GetPhysicalDeviceRef()->Get(), &Props);

        info.maxAnisotropy = Props.limits.maxSamplerAnisotropy;
        info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.mipLodBias = 0.f;
        info.minLod = 0.f;
        info.maxLod = 1;

        _VKR = vkCreateSampler(VulkanContext::Get()->GetDeviceRef()->Get(), &info, nullptr, &m_Sampler);
    }

    m_Swapchain = VulkanSwapchain::Get();
    m_DescriptorSets.resize(m_Swapchain->GetImageCount());
}

Nexus::VulkanEditorViewport::~VulkanEditorViewport()
{
    vkDestroySampler(VulkanContext::Get()->GetDeviceRef()->Get(), m_Sampler, nullptr);
}

void Nexus::VulkanEditorViewport::SetContext(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex)
{
    m_Framebuffer = DynamicPointerCast<VulkanFramebuffer>(framebuffer);
    m_Extent = m_Swapchain->GetImageExtent();

    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    uint32_t count = m_Swapchain->GetImageCount();
       
    // Create Descriptor Set:
    if (m_DescriptorSets[0] != nullptr)
    {
        vkFreeDescriptorSets(VulkanContext::Get()->GetDeviceRef()->Get(), v->DescriptorPool, (uint32_t)m_DescriptorSets.size(), m_DescriptorSets.data());
    }

    std::vector<VkDescriptorSetLayout> lays(count);
    for (uint32_t i = 0; i < count; i++)
    {
        lays[i] = bd->DescriptorSetLayout;
    }

    m_layout = &bd->PipelineLayout;

    {
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = v->DescriptorPool;
        alloc_info.descriptorSetCount = count;
        alloc_info.pSetLayouts = lays.data();
        VkResult err = vkAllocateDescriptorSets(v->Device, &alloc_info, m_DescriptorSets.data());
    }

    std::vector<VkDescriptorImageInfo> imageInfos(count);
    std::vector<VkWriteDescriptorSet> writes(count);

    for (uint32_t i = 0; i < count; i++)
    {
        imageInfos[i].sampler = m_Sampler;
        imageInfos[i].imageView = m_Framebuffer->GetAttachmentView(i, attachmentIndex);
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = m_DescriptorSets[i];
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].pImageInfo = &imageInfos[i];

    }
    vkUpdateDescriptorSets(v->Device, count, writes.data(), 0, NULL);
}

void Nexus::VulkanEditorViewport::Render()
{
    VkCommandBuffer cmd = VulkanCommandQueue::Get()->GetCurrentCommandBuffer();
    uint32_t frameIndex = VulkanCommandQueue::Get()->GetFrameIndex();

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_layout, 0, 1, &m_DescriptorSets[frameIndex], 0, nullptr);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");

    m_Panelsize = ImGui::GetContentRegionAvail();

    ImGui::Image(m_DescriptorSets[frameIndex], m_Panelsize);
   
    ImGui::End();
    ImGui::PopStyleVar();
}

glm::vec2 Nexus::VulkanEditorViewport::GetViewportSize()
{
    return { m_Panelsize.x,m_Panelsize.y };
}
