#include "nxpch.h"
#include "Command.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkCommand.h"

Nexus::Ref<Nexus::Command> s_Command;

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

void Nexus::Command::SetPushConstantData(Ref<Pipeline> pipeline, void* data, uint32_t size)
{
	s_Command->ImplSetPushConstantData(pipeline, data, size);
}

void Nexus::Command::Draw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	s_Command->ImplDraw(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void Nexus::Command::DrawIndexed(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance)
{
	s_Command->ImplDrawIndexed(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}
