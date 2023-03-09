#include "nxpch.h"
#include "RenderCommand.h"

#include "RenderAPI.h"
#include "Platform/Vulkan/VkRenderCommand.h"

Nexus::Ref<Nexus::RenderCommand> s_RenderCommand;

void Nexus::RenderCommand::Init()
{
	switch (RenderAPI::GetCurrentAPI())
	{
		case RenderAPIType::VULKAN: s_RenderCommand = CreateRef<VulkanRenderCommand>(); break;
		case RenderAPIType::NONE: return;
		default: return;
	}
}

void Nexus::RenderCommand::Shut()
{
	s_RenderCommand.reset();
}

void Nexus::RenderCommand::Update()
{
	s_RenderCommand->ImplUpdate();
}

void Nexus::RenderCommand::BindPipeline(Ref<Pipeline> pipeline)
{
	s_RenderCommand->ImplBindPipeline(pipeline);
}

void Nexus::RenderCommand::SetViewport(const Viewport& viewport)
{
	s_RenderCommand->ImplSetViewport(viewport);
}

void Nexus::RenderCommand::SetScissor(const Scissor& scissor)
{
	s_RenderCommand->ImplSetScissor(scissor);
}

void Nexus::RenderCommand::Draw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	s_RenderCommand->ImplDraw(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

