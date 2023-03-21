#pragma once
#include "RendererUtils.h"

#include "Renderer/Pipeline.h"

#include "Assets/Mesh.h"

namespace Nexus
{
	class Command
	{
	public:
		static void Init();
		static void Shut();

		static void Update();

		static void TransferStaticMesh(Ref<StaticMesh> mesh);

		static void SetPushConstantData(Ref<Pipeline> pipeline, void* data, uint32_t size);
		static void SetViewport(const Viewport& viewport);
		static void SetScissor(const Scissor& scissor);
		
		static void BindPipeline(Ref<Pipeline> pipeline);
		static void DrawMesh(Ref<StaticMesh> mesh);

		static void Draw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance);
		static void DrawIndexed(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance);
	private:
		virtual void ImplInit() = 0;
		virtual void ImplUpdate() = 0;

		virtual void ImplTransferStaticMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplBindPipeline(Ref<Pipeline> pipeline) = 0;
		virtual void ImplDrawMesh(Ref<StaticMesh> mesh) = 0;
		virtual void ImplSetViewport(const Viewport& viewport) = 0;
		virtual void ImplSetScissor(const Scissor& scissor) = 0;
		virtual void ImplSetPushConstantData(Ref<Pipeline> pipeline, void* data, uint32_t size) = 0;
		virtual void ImplDraw(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) = 0;
		virtual void ImplDrawIndexed(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance) = 0;
	};
}