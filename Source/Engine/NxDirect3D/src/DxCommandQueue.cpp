#include "NxDirect3D/DxCommandQueue.h"
#include "NxDirect3D/DxContext.h"
#include "NxDirect3D/DxSwapchain.h"
#include "NxDirect3D/DxFramebuffer.h"
#include "NxDirect3D/DxRenderpass.h"

Nexus::DirectXCommandQueue* Nexus::DirectXCommandQueue::s_Instance = nullptr;

Nexus::DirectXCommandQueue::DirectXCommandQueue(std::function<void()> resizeCallback)
{
	m_ResizeCallback = resizeCallback;
	s_Instance = this;
}

void Nexus::DirectXCommandQueue::Initialize()
{
	auto context = DirectXContext::Get();
	auto swapchain = DirectXSwapchain::Get();

	auto device = context->GetDevice();

	m_Swapchain = swapchain->GetHandle();
	m_Swapchain->AddRef();

	// Queue
	{
		m_GraphicsQueue = context->GetGraphicsQueue();
		m_GraphicsQueue->AddRef();

		m_CopyQueue = context->GetCopyQueue();
		m_CopyQueue->AddRef();

		m_ComputeQueue = context->GetComputeQueue();
		m_ComputeQueue->AddRef();
	}

	// Command Buffers
	{
		auto count = swapchain->GetImageCount();

		m_fenceValues.resize(count);

		m_GraphicsCommandBuffer.resize(count);
		m_CopyCommandBuffer.resize(count);
		m_ComputeCommandBuffer.resize(count);

		for (uint32_t i = 0; i < count; i++)
		{
			auto& gb = m_GraphicsCommandBuffer[i];
			CHECK_CALL(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&gb.alloc));
			CHECK_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,gb.alloc, nullptr,__uuidof(ID3D12GraphicsCommandList1), (void**)&gb.buffer));

			auto& cb = m_CopyCommandBuffer[i];
			CHECK_CALL(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator), (void**)&cb.alloc));
			CHECK_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY,cb.alloc, nullptr,__uuidof(ID3D12GraphicsCommandList1), (void**)&cb.buffer));
			
			auto& cmb = m_ComputeCommandBuffer[i];
			CHECK_CALL(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, __uuidof(ID3D12CommandAllocator), (void**)&cmb.alloc));
			CHECK_CALL(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE,cmb.alloc, nullptr,__uuidof(ID3D12GraphicsCommandList1), (void**)&cmb.buffer));
		}

		NEXUS_LOG("DirectX", "Command Buffers Created");
	}
}

void Nexus::DirectXCommandQueue::Shutdown()
{
	auto count = m_GraphicsCommandBuffer.size();

	for (auto i = 0; i < count; i++)
	{
		m_GraphicsCommandBuffer[i].alloc->Release();
		m_GraphicsCommandBuffer[i].buffer->Release();
		
		m_ComputeCommandBuffer[i].alloc->Release();
		m_ComputeCommandBuffer[i].buffer->Release();
		
		m_CopyCommandBuffer[i].alloc->Release();
		m_CopyCommandBuffer[i].buffer->Release();
	}	
	NEXUS_LOG("DirectX", "Command Buffers Destroyed");

	m_GraphicsQueue->Release();
	m_ComputeQueue->Release();
	m_CopyQueue->Release();

	m_Swapchain->Release();
}

void Nexus::DirectXCommandQueue::StartComputeQueue()
{
}

void Nexus::DirectXCommandQueue::FlushComputeQueue()
{
}

void Nexus::DirectXCommandQueue::BeginRenderQueue()
{
	m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();

	auto list = m_GraphicsCommandBuffer[m_FrameIndex].buffer;
	auto alloc = m_GraphicsCommandBuffer[m_FrameIndex].alloc;

	// use fence here
	CHECK_CALL(alloc->Reset());
	
	CHECK_CALL(list->Reset(alloc, nullptr));
}

void Nexus::DirectXCommandQueue::EndRenderQueue()
{
}

void Nexus::DirectXCommandQueue::FlushRenderQueue()
{
}

void Nexus::DirectXCommandQueue::FlushTransferQueue()
{
}

void Nexus::DirectXCommandQueue::BeginRenderPass(Ref<Renderpass> pass, Ref<Framebuffer> framebuffer)
{
	
}

void Nexus::DirectXCommandQueue::EndRenderPass()
{
}

void Nexus::DirectXCommandQueue::BindShaderResourceHeap(Ref<Shader> shader, ResourceHeapHandle handle, PipelineBindPoint bind)
{
}

void Nexus::DirectXCommandQueue::BindPipeline(Ref<Pipeline> pipeline)
{
}

void Nexus::DirectXCommandQueue::SetScissor(Scissor scissor)
{
}

void Nexus::DirectXCommandQueue::SetViewport(Viewport viewport)
{
}

void Nexus::DirectXCommandQueue::SetPushConstant(Ref<Shader> shader, PipelineBindPoint bindPoint, ShaderStage stage, void* data)
{
}

void Nexus::DirectXCommandQueue::BindVertexBuffer(Ref<Buffer> buffer)
{
}

void Nexus::DirectXCommandQueue::BindIndexBuffer(Ref<Buffer> buffer)
{
}

void Nexus::DirectXCommandQueue::DrawVertices(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
}

void Nexus::DirectXCommandQueue::DrawIndices(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, uint32_t VertexOffset, uint32_t FirstInstance)
{
}

void Nexus::DirectXCommandQueue::Dispatch(uint32_t xWorkers, uint32_t yWorkers, uint32_t zWorkers)
{
}
