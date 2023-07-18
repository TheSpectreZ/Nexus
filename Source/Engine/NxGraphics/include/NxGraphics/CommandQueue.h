#pragma once
#include <functional>

#include "TypeImpls.h"
#include "Renderpass.h"
#include "Framebuffer.h"	
#include "Pipeline.h"
#include "Buffer.h"

namespace Nexus
{
	class CommandQueue
	{
	public:
		CommandQueue() = default;
		virtual ~CommandQueue() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual uint32_t GetFrameIndex() = 0;

		virtual void BeginRenderQueue() = 0;
		virtual void EndRenderQueue() = 0;
		
		virtual void StartComputeQueue() = 0;
		virtual void FlushComputeQueue() = 0;

		virtual void FlushTransferQueue() = 0;
		virtual void FlushRenderQueue() = 0;

		virtual void BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer) = 0;
		virtual void EndRenderPass() = 0;

		virtual void BindShaderResourceHeap(Ref<Shader> shader, ResourceHeapHandle handle, PipelineBindPoint bind) = 0;
		virtual void BindPipeline(Ref<Pipeline> pipeline) = 0;

		virtual void SetScissor(Scissor scissor) = 0;
		virtual void SetViewport(Viewport viewport) = 0;
		virtual void SetPushConstant(Ref<Shader> shader, PipelineBindPoint bindPoint, ShaderStage stage, void* data) = 0;

		virtual void BindVertexBuffer(Ref<Buffer> buffer) = 0;
		virtual void BindIndexBuffer(Ref<Buffer> buffer) = 0;

		virtual void DrawIndices(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance) = 0;
		virtual void Dispatch(uint32_t xWorker, uint32_t yWorker, uint32_t zWorker) = 0;
	};
}