#pragma once
#include "NxGraphics/CommandQueue.h"
#include "DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif // NEXUS_DIRECTX_SHARED_BUILD

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXCommandQueue : public CommandQueue
	{
		static DirectXCommandQueue* s_Instance;
	public:
		static DirectXCommandQueue* Get() { return s_Instance; }

		DirectXCommandQueue(std::function<void()> resizeCallback);
		~DirectXCommandQueue() = default;

		void Initialize() override;
		void Shutdown() override;

		virtual uint32_t GetFrameIndex() { return 0; }

		void StartComputeQueue() override;
		void FlushComputeQueue() override;

		void BeginRenderQueue() override;
		void EndRenderQueue() override;
		void FlushRenderQueue() override;

		void FlushTransferQueue() override;

		void BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer) override;
		void EndRenderPass() override;

		void BindShaderResourceHeap(Ref<Shader> shader, ResourceHeapHandle handle, PipelineBindPoint bind) override;
		void BindPipeline(Ref<Pipeline> pipeline) override;

		void SetScissor(Scissor scissor) override;
		void SetViewport(Viewport viewport) override;
		void SetPushConstant(Ref<Shader> shader, PipelineBindPoint bindPoint, ShaderStage stage, void* data) override;

		void BindVertexBuffer(Ref<Buffer> buffer) override;
		void BindIndexBuffer(Ref<Buffer> buffer) override;

		void DrawVertices(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
		void DrawIndices(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance) override;
		
		void Dispatch(uint32_t xWorkers, uint32_t yWorkers, uint32_t zWorkers) override;
	private:
		std::function<void()> m_ResizeCallback;

		uint32_t m_FrameIndex, m_ImageIndex, m_ImageCount;
	
		IDXGISwapChain3* m_Swapchain;

		ID3D12CommandQueue* m_GraphicsQueue;
		ID3D12CommandQueue* m_CopyQueue;
		ID3D12CommandQueue* m_ComputeQueue;

		struct CommandBuffer
		{
			ID3D12CommandAllocator* alloc;
			ID3D12GraphicsCommandList1* buffer;
		};

		std::vector<CommandBuffer> m_GraphicsCommandBuffer;
		std::vector<CommandBuffer> m_CopyCommandBuffer;
		std::vector<CommandBuffer> m_ComputeCommandBuffer;

		std::vector<uint64_t> m_fenceValues;

		HANDLE m_fenceEvent;
		ID3D12Fence* m_fence;
	};
}
