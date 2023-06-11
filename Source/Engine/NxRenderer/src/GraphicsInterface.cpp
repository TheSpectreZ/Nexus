#include "NxRenderer/GraphicsInterface.h"
#include "NxCore/Assertion.h"

#include "NxVulkan/VkContext.h"
#include "NxVulkan/VkSwapchain.h"
#include "NxVulkan/VkCommandQueue.h"

namespace Nexus::GraphicsInterface
{
	static Nexus::RendererAPI s_API;
}

#define NEXUS_INTERFACE_SWITCH(VulkanReturn) switch(Nexus::GraphicsInterface::s_API)\
{\
	case Nexus::RendererAPI::VULKAN:\
		return VulkanReturn;\
	case Nexus::RendererAPI::NONE:\
		return nullptr;\
	default:\
		return nullptr;\
}

Nexus::Ref<Nexus::Context> Nexus::GraphicsInterface::CreateContext(RendererAPI API, const ContextCreateInfo& Info)
{
	NEXUS_BREAK_ASSERT((API == RendererAPI::NONE), "Selected RenderAPI is None");
	s_API = API;
	
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanContext>(Info));
}

Nexus::Ref<Nexus::Swapchain> Nexus::GraphicsInterface::CreateSwapchain(Window* window)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanSwapchain>(window));
}

Nexus::Ref<Nexus::CommandQueue> Nexus::GraphicsInterface::CreateCommandQueue(std::function<void()> resizeCallback)
{
	NEXUS_INTERFACE_SWITCH(CreateRef<VulkanCommandQueue>(resizeCallback));
}
