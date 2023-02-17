#include "Graphics/Engine.h"

#include "Backend.h"
#include "Presenter.h"

void Nexus::Graphics::Engine::Initialize(const EngineSpecification& Specs, std::function<void(uint32_t, uint32_t)> ResizeCallback)
{
	Backend::Get().Init(Specs);
	
	Presenter::Get().Init(Specs);
	Presenter::Get().WindowResizeCallbackFnc = ResizeCallback;
}

void Nexus::Graphics::Engine::Shutdown()
{
	WaitForDevice();

	Presenter::Get().Shut();
	Backend::Get().Shut();
}

void Nexus::Graphics::Engine::WaitForDevice()
{
	vkDeviceWaitIdle(Backend::Get().m_Device);
}

VkCommandBuffer Nexus::Graphics::Engine::StartFrame()
{
	Presenter::Get().StartFrame();
	return Presenter::Get().m_CommandBuffers[Presenter::s_CurrentFrame];
}

void Nexus::Graphics::Engine::EndFrame()
{
	Presenter::Get().EndFrame();
}

VkCommandBuffer Nexus::Graphics::Engine::BeginSingleTimeCommandRecording()
{
	return Backend::Get().BeginSingleTimeCommands();
}

void Nexus::Graphics::Engine::EndSingleTimeCommandRecording(VkCommandBuffer cmd)
{
	Backend::Get().EndSingleTimeCommands(cmd);
}

#pragma region Getters

VkInstance Nexus::Graphics::Engine::GetInstance() { return Backend::Get().m_Instance; }

VkPhysicalDevice Nexus::Graphics::Engine::GetGpuHandle() { return  Backend::Get().m_PhysicalDevice; }

VkDevice Nexus::Graphics::Engine::GetDevice() { return  Backend::Get().m_Device; }

VkSurfaceKHR Nexus::Graphics::Engine::GetSurface() { return  Backend::Get().m_Surface; }

VkQueue Nexus::Graphics::Engine::GetGraphicsQueue() { return  Backend::Get().m_GraphicsQueue; }

VkQueue Nexus::Graphics::Engine::GetPresentQueue() { return  Backend::Get().m_PresentQueue; }

std::pair<uint32_t, uint32_t> Nexus::Graphics::Engine::GetQueueFamilyIndices()
{
	QueueIndexFamilies fam = GetQueueIndexFamilies(Backend::Get().m_PhysicalDevice,Backend::Get().m_Surface);
	return { fam[0].value(),fam[1].value() };
}

VmaAllocator Nexus::Graphics::Engine::GetAllocator() { return  Backend::Get().m_VmaAllocator; }

VkSampleCountFlagBits Nexus::Graphics::Engine::GetMaxSampleCount()	{ return  Backend::Get().m_Msaa; }

VkCommandBuffer Nexus::Graphics::Engine::GetCurrentRenderCommandBuffer() { return Presenter::Get().m_CommandBuffers[Presenter::s_CurrentFrame]; }

VkImageView Nexus::Graphics::Engine::GetSwapChainImageView(uint32_t i) { return Presenter::Get().m_ImageViews[i]; }

VkExtent2D Nexus::Graphics::Engine::GetSwapchainImageExtent() { return Presenter::Get().m_SwapchainExtent; }

VkFormat Nexus::Graphics::Engine::GetSwapchainImageFormat() { return Presenter::Get().m_SwapchainImageFormat; }

uint32_t Nexus::Graphics::Engine::GetMaxSwapchainImageCount() { return Presenter::s_FramesInFlight; }

uint32_t Nexus::Graphics::Engine::GetCurrentFrameIndex() { return Presenter::s_CurrentFrame; }

VkSwapchainKHR Nexus::Graphics::Engine::GetSwapchain() { return Presenter::Get().m_Swapchain; }

#pragma endregion
