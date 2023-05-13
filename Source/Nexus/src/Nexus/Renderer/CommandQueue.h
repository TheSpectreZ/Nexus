#pragma once
#include "Mesh.h"
#include "Framebuffer.h"
#include "Pipeline.h"

namespace Nexus
{
	class CommandQueue
	{
	public:
		static Ref<CommandQueue> Create();

		CommandQueue() = default;
		virtual ~CommandQueue() = default;

		virtual void Init() = 0;
		virtual void Shut() = 0;

		virtual uint32_t GetFrameIndex() = 0;

		virtual void BeginRenderQueue() = 0;
		virtual void EndRenderQueue() = 0;
		virtual void FlushRenderQueue() = 0;
		
		virtual void FlushTransferQueue() = 0;

		virtual void BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer) = 0;
		virtual void EndRenderPass() = 0;

		virtual void BindPipeline(Ref<Pipeline> pipeline) = 0;

		virtual void SetScissor(Scissor scissor) = 0;
		virtual void SetViewport(Viewport viewport) = 0;

		virtual void TransferMeshToGPU(Ref<StaticMesh> mesh) = 0;
		virtual void TransferTextureToGPU(Ref<Texture> texture) = 0;

		virtual void DrawMesh(Ref<StaticMesh> mesh) = 0;
	};
}
