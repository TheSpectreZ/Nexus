#include "nxpch.h"
#include "EditorViewport.h"

#include "Renderer/RenderAPI.h"
#include "Platform/Vulkan/VulkanEditorViewport.h"

Nexus::Ref<Nexus::EditorViewport> Nexus::EditorViewport::Create()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: return CreateRef<VulkanEditorViewport>();
		case RenderAPIType::NONE: return nullptr;
		default:return nullptr;
	}
}
