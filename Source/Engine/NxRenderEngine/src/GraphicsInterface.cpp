#include "NxRenderEngine/GraphicsInterface.h"
#include "NxCore/Assertion.h"

#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkCommandQueue.h"
#include "NxVulkan/VkRenderpass.h"
#include "NxVulkan/VkFramebuffer.h"
#include "NxVulkan/VkPipeline.h"
#include "NxVulkan/VkShader.h"

#include "NxDirect3D/DxContext.h"
#include "NxDirect3D/DxSwapchain.h"
#include "NxDirect3D/DxCommandQueue.h"
#include "NxDirect3D/DxShader.h"
#include "NxDirect3D/DxRenderpass.h"
#include "NxDirect3D/DxFramebuffer.h"

namespace Nexus::GraphicsInterface
{
	static Nexus::RendererAPI s_API;
}

#define NEXUS_INTERFACE_SWITCH(VulkanReturn, DirectXReturn) switch(Nexus::GraphicsInterface::s_API)\
{\
	case Nexus::RendererAPI::VULKAN:\
		return VulkanReturn;\
	case Nexus::RendererAPI::DIRECTX:\
		return DirectXReturn;\
	case Nexus::RendererAPI::NONE:\
		return nullptr;\
	default:\
		return nullptr;\
}

Nexus::Ref<Nexus::Context> Nexus::GraphicsInterface::CreateContext(RendererAPI API, const ContextCreateInfo& Info)
{
	NEXUS_BREAK_ASSERT((API == RendererAPI::NONE), "Selected RenderAPI is None");
	s_API = API;
	
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanContext>(Info), CreateRef<DirectXContext>(Info));
}

Nexus::Ref<Nexus::Swapchain> Nexus::GraphicsInterface::CreateSwapchain(Window* window)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanSwapchain>(window), CreateRef<DirectXSwapchain>(window));
}

Nexus::Ref<Nexus::CommandQueue> Nexus::GraphicsInterface::CreateCommandQueue(std::function<void()> resizeCallback)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanCommandQueue>(resizeCallback), CreateRef<DirectXCommandQueue>(resizeCallback));
}

Nexus::Ref<Nexus::Shader> Nexus::GraphicsInterface::CreateShader(const ShaderSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanShader>(specs), CreateRef<DirectXShader>(specs));
}

Nexus::Ref<Nexus::Renderpass> Nexus::GraphicsInterface::CreateRenderpass(const RenderpassSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanRenderpass>(specs), CreateRef<DirectXRenderpass>(specs));
}

Nexus::Ref<Nexus::Framebuffer> Nexus::GraphicsInterface::CreateFramebuffer(const FramebufferSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanFramebuffer>(specs), CreateRef<DirectXFramebuffer>(specs));
}

Nexus::Ref<Nexus::Pipeline> Nexus::GraphicsInterface::CreatePipeline(const GraphicsPipelineSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanPipeline>(specs),CreateRef<VulkanPipeline>(specs));
}

Nexus::Ref<Nexus::Pipeline> Nexus::GraphicsInterface::CreatePipeline(const ComputePipelineSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanPipeline>(specs), CreateRef<VulkanPipeline>(specs));
}

Nexus::Ref<Nexus::Buffer> Nexus::GraphicsInterface::CreateBuffer(const BufferSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanBuffer>(specs), CreateRef<VulkanBuffer>(specs));
}

Nexus::Ref<Nexus::Texture>NEXUS_RENDERER_API Nexus::GraphicsInterface::CreateTexture(const TextureSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanTexture>(specs), CreateRef<VulkanTexture>(specs));
}

Nexus::Ref<Nexus::Sampler>NEXUS_RENDERER_API Nexus::GraphicsInterface::CreateSampler(const SamplerSpecification& specs)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanSampler>(specs), CreateRef<VulkanSampler>(specs));
}
