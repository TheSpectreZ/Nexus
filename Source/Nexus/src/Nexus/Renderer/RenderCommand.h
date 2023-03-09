#pragma once
#include "RendererUtils.h"

#include "Renderer/Pipeline.h"

namespace Nexus
{
	class RenderCommand
	{
	public:
		static void Init();
		static void Shut();

		static void Update();
		static void BindPipeline(Ref<Pipeline> pipeline);
		static void SetViewport(const Viewport& viewport);
		static void SetScissor(const Scissor& scissor);
		
		static void Draw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance);
	private:
		virtual void ImplUpdate() = 0;
		virtual void ImplBindPipeline(Ref<Pipeline> pipeline) = 0;
		virtual void ImplSetViewport(const Viewport& viewport) = 0;
		virtual void ImplSetScissor(const Scissor& scissor) = 0;
		virtual void ImplDraw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) = 0;
	};
}