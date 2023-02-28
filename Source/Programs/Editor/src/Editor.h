#pragma once
#include <cstdint>
#include <unordered_map>

#include "Config/EditorConfig.h"
#include "Platform/Window.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Renderpass.h"
#include "Graphics/Descriptor.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Resource.h"

using namespace Nexus;
using namespace Nexus::Platform;
using namespace Nexus::Graphics;
using namespace Nexus::Config;

template<typename T>
using StringHash = std::unordered_map<std::string, T>;

class Editor
{
public:
	void Initialize();
	void Run();
	void Shutdown();
private:
	void OnWindowResize(uint32_t width, uint32_t height);
	Window m_Window;

	StringHash<Renderpass> m_Renderpass;
	StringHash<FramebufferAttachment> m_FramebufferAttachment;
	StringHash<std::vector<Framebuffer>> m_Framebuffer;
	
	StringHash<DescriptorPool> m_DescriptorPool;
	StringHash<DescriptorLayout> m_DescriptorLayout;

	StringHash<PipelineLayout> m_PipelineLayout;
	StringHash<GraphicsPipeline> m_GraphicsPipeline;
	
	Sampler m_ImguiSampler;
	StringHash<std::vector<VkClearValue>> m_ClearValues;
	std::vector<VkDescriptorSet> m_OffScreenDescriptor;

	void GraphicsCommandRecording(VkCommandBuffer cmd);
	void ImGuiCommandRecording(VkCommandBuffer cmd);

	void BuildRenderpass();
	void BuildAttachment();
	void BuildFramebuffer();
	void BuildDescriptor();
	
	void SetupImGui();
	void ShutdownImGui();
};

