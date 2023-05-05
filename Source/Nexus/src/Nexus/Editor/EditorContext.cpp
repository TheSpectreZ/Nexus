#include "nxpch.h"
#include "EditorContext.h"

#include "Renderer/RenderAPI.h"
#include "Platform/Vulkan/VulkanEditorContext.h"

Nexus::EditorContext* Nexus::EditorContext::s_Instance = nullptr;

void Nexus::EditorContext::Initialize(Ref<Renderpass> renderpass)
{
	switch (RenderAPI::GetCurrentAPI())
	{
	case Nexus::RenderAPIType::VULKAN:
		s_Instance = new VulkanEditorContext(renderpass);
		break;
	case Nexus::RenderAPIType::NONE:
		s_Instance = nullptr;
		break;
	default:
		s_Instance = nullptr;
		break;
	}

	NEXUS_ASSERT((s_Instance == nullptr), "Editor Context Initialization Failed");
}

void Nexus::EditorContext::Shutdown()
{
	delete s_Instance;
}

void Nexus::EditorContext::StartFrame()
{
	s_Instance->Start();
}

void Nexus::EditorContext::Render()
{
	s_Instance->End();
}
