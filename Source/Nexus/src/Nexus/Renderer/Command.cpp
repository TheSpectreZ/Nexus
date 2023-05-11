#include "nxpch.h"
#include "Command.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkCommand.h"

Nexus::Ref<Nexus::Command> s_Command;

Nexus::Ref<Nexus::Command> Nexus::Command::GetRef()
{
	return s_Command;
}

void Nexus::Command::Init()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: s_Command = CreateRef<VulkanCommand>(); break;
		case RenderAPIType::NONE: return;
		default: return;
	}
	s_Command->ImplInit();
}

void Nexus::Command::Shut()
{
	s_Command.reset();
}

void Nexus::Command::Update()
{
	s_Command->ImplUpdate();
}

void Nexus::Command::TransferStaticMesh(Ref<StaticMesh> mesh)
{
	s_Command->ImplTransferStaticMesh(mesh);
}

void Nexus::Command::TransferMaterial(Ref<Material> material)
{
	s_Command->ImplTransferMaterial(material);
}

void Nexus::Command::BindPipeline(Ref<Pipeline> pipeline)
{
	s_Command->ImplBindPipeline(pipeline);
}

void Nexus::Command::DrawMesh(Ref<StaticMesh> mesh)
{
	s_Command->ImplDrawMesh(mesh);
}

void Nexus::Command::SetViewport(const Viewport& viewport)
{
	s_Command->ImplSetViewport(viewport);
}

void Nexus::Command::SetScissor(const Scissor& scissor)
{
	s_Command->ImplSetScissor(scissor);
}

void Nexus::Command::BeginRenderpass(Ref<Renderpass> renderpass, Ref<Framebuffer> framebuffer)
{
	s_Command->ImplBeginRenderpass(renderpass, framebuffer);
}

void Nexus::Command::EndRenderpass()
{
	s_Command->ImplEndRenderpass();
}
