#include "NxVulkan/VkRenderpass.h"
#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"

static VkAttachmentLoadOp GetVulkanLoadOperations(Nexus::ImageOperation op)
{
	switch (op)
	{
	case Nexus::ImageOperation::Load:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	case Nexus::ImageOperation::Clear:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case Nexus::ImageOperation::DontCare:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	default:
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

static VkAttachmentStoreOp GetVulkanStoreOperations(Nexus::ImageOperation op)
{
	switch (op)
	{
	case Nexus::ImageOperation::Store:
		return VK_ATTACHMENT_STORE_OP_STORE;
	case Nexus::ImageOperation::DontCare:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

static VkImageLayout GetVulkanImageLayout(Nexus::ImageLayout layout)
{
	switch (layout)
	{
	case Nexus::ImageLayout::ColorAttachment:
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case Nexus::ImageLayout::DepthAttachment:
		return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	case Nexus::ImageLayout::DepthStencilAttachment:
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	case Nexus::ImageLayout::ShaderReadOnly:
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case Nexus::ImageLayout::TransferSrc:
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case Nexus::ImageLayout::TransferDst:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case Nexus::ImageLayout::PresentSrc:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	case Nexus::ImageLayout::Undefined:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	default:
		return VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}

static VkPipelineStageFlags GetVulkanPipelineStageFlag(Nexus::PipelineStageFlag flag)
{
	VkPipelineStageFlags output = 0;

	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::TopOfPipe))
		output |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::BottomOfPipe))
		output |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::VertexInput))
		output |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::VertexShader))
		output |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::FragmentShader))
		output |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::EarlyFragmentTest))
		output |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::LateFragmentTest))
		output |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::ColorAttachmentOutput))
		output |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::TransferBit))
		output |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::ComputeBit))
		output |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::PipelineStageFlag::None))
		output |= VK_PIPELINE_STAGE_NONE;

	return output;
}

static VkAccessFlags GetVulkanAccessFlags(Nexus::AccessFlag flag)
{
	VkAccessFlags output = 0;

	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::IndexRead))
		output |= VK_ACCESS_INDEX_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::UniformRead))
		output |= VK_ACCESS_UNIFORM_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::InputAttachmentRead))
		output |= VK_ACCESS_INDEX_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::ShaderRead))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::ShaderWrite))
		output |= VK_ACCESS_SHADER_WRITE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::ColorAttachmentRead))
		output |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::ColorAttachmentWrite))
		output |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::DepthStencilAttachmentRead))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::DepthStencilAttachmentWrite))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::TransferRead))
		output |= VK_ACCESS_TRANSFER_READ_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::TransferWrite))
		output |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (Nexus::IsFlagSet(flag, Nexus::AccessFlag::None))
		output |= VK_ACCESS_NONE;

	return output;
}

Nexus::VulkanRenderpass::VulkanRenderpass(const RenderpassSpecification& specs)
{
	Ref<VulkanPhysicalDevice> gpu = VulkanContext::Get()->GetPhysicalDeviceRef();
	VulkanSwapchain* swapchain = VulkanSwapchain::Get();
	Ref<VulkanDevice> dev = VulkanContext::Get()->GetDeviceRef();

	// Pass
	{
		VkRenderPassCreateInfo Info{};

		std::vector<VkAttachmentDescription> aDesc;	
		for (uint32_t i = 0; i < specs.attachments->size(); i++)
		{
			auto& atc = specs.attachments->at(i);

			auto& d = aDesc.emplace_back();

			switch (atc.type)
			{
			case ImageType::Color:
				if (atc.finalLayout == ImageLayout::PresentSrc)
					d.format = swapchain->GetImageFormat();
				else
					d.format = atc.hdr ? VK_FORMAT_R32G32B32A32_SFLOAT : swapchain->GetImageFormat();
				break;
			case ImageType::Depth:
				d.format = gpu->GetDepthFormat();
				break;
			case ImageType::Resolve:
				d.format = atc.hdr ? VK_FORMAT_R32G32B32A32_SFLOAT : swapchain->GetImageFormat();
				break;
			default:
				d.format = VK_FORMAT_MAX_ENUM;
				break;
			};

			d.samples = atc.multiSampled ? gpu->GetMaxSampleCount() : VK_SAMPLE_COUNT_1_BIT;
			d.loadOp = GetVulkanLoadOperations(atc.load);
			d.storeOp = GetVulkanStoreOperations(atc.store);
			d.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			d.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			d.initialLayout = GetVulkanImageLayout(atc.initialLayout);
			d.finalLayout = GetVulkanImageLayout(atc.finalLayout);
			d.flags = 0;
		}

		std::unordered_map<uint32_t, std::vector<std::vector<VkAttachmentReference>>> Refs;

		std::vector<VkSubpassDescription> sDesc;
		for (uint32_t i = 0; i < specs.subpasses->size(); i++)
		{
			auto& sub = specs.subpasses->at(i);

			auto& list = Refs[i];

			auto& s = sDesc.emplace_back();
			//[Note]: Check ref.layout if Subpass Throw some error, They are Hard-coded... Generalize it if needed.
			if (!sub.ColorAttachments.empty())
			{
				s.colorAttachmentCount = (uint32_t)sub.ColorAttachments.size();

				auto& colorRefs = list.emplace_back();
				for (uint32_t j = 0; j < s.colorAttachmentCount; j++)
				{
					auto& ref = colorRefs.emplace_back();
					ref.attachment = sub.ColorAttachments[j];
					ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				s.pColorAttachments = colorRefs.data();
			}
			else
			{
				s.colorAttachmentCount = 0;
				s.pColorAttachments = nullptr;
			}

			if (sub.DepthAttachment >= 0)
			{
				auto& depthRefs = list.emplace_back();
				for (uint32_t j = 0; j < 1; j++)
				{
					auto& ref = depthRefs.emplace_back();
					ref.attachment = sub.DepthAttachment;
					ref.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
				}
				s.pDepthStencilAttachment = depthRefs.data();
			}
			else
			{
				s.pDepthStencilAttachment = nullptr;
			}

			if (sub.ResolveAttachment >= 0)
			{
				auto& resolveRefs = list.emplace_back();
				for (uint32_t j = 0; j < 1; j++)
				{
					auto& ref = resolveRefs.emplace_back();
					ref.attachment = sub.ResolveAttachment;
					ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				s.pResolveAttachments = resolveRefs.data();
			}
			else
			{
				s.pResolveAttachments = nullptr;
			}

			if (!sub.InputAttachments.empty())
			{
				s.inputAttachmentCount = (uint32_t)sub.InputAttachments.size();

				auto& inputRefs = list.emplace_back();
				for (uint32_t j = 0; j < s.inputAttachmentCount; j++)
				{
					auto& ref = inputRefs.emplace_back();
					ref.attachment = sub.InputAttachments[j];
				}
				s.pInputAttachments = inputRefs.data();
			}
			else
			{
				s.inputAttachmentCount = 0;
				s.pInputAttachments = nullptr;
			}
			
			// Currently Not Supported
			s.preserveAttachmentCount = 0;
			s.pPreserveAttachments = nullptr;

			s.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		}

		std::vector<VkSubpassDependency> dDesc;
		for (uint32_t i = 0; i < specs.dependencies->size(); i++)
		{
			auto& dep = specs.dependencies->at(i);

			auto& d = dDesc.emplace_back();
			d.srcSubpass = dep.srcSubpass;
			d.dstSubpass = dep.dstSubpass;
			d.srcStageMask = GetVulkanPipelineStageFlag(dep.srcStageFlags);
			d.dstStageMask = GetVulkanPipelineStageFlag(dep.dstStageFlags);
			d.srcAccessMask = GetVulkanAccessFlags(dep.srcAccessFlags);
			d.dstAccessMask = GetVulkanAccessFlags(dep.dstAccessFlags);
		}

		Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		Info.pAttachments = aDesc.data();
		Info.attachmentCount = (uint32_t)aDesc.size();
		Info.pSubpasses = sDesc.data();
		Info.subpassCount = (uint32_t)sDesc.size();
		Info.pDependencies = dDesc.data();
		Info.dependencyCount = (uint32_t)dDesc.size();

		_VKR = vkCreateRenderPass(dev->Get(), &Info, nullptr, &m_Pass);
		CHECK_LOG_VKR;
		NEXUS_LOG("Vulkan", "Renderpass Created");
	}
}

Nexus::VulkanRenderpass::~VulkanRenderpass()
{
	Ref<VulkanDevice> dev = VulkanContext::Get()->GetDeviceRef();

	vkDestroyRenderPass(dev->Get(), m_Pass, nullptr);
	NEXUS_LOG("Vulkan", "Renderpass Destroyed");
}
