#pragma once
#include "NxGraphics/Context.h"
#include "NxDirect3D/DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif // NEXUS_DIRECTX_SHARED_BUILD

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXContext : public Context
	{
		static DirectXContext* s_Instance;
	public:
		DirectXContext(const ContextCreateInfo& Info);
		~DirectXContext() override
		{
			s_Instance = nullptr;
		}

		void Initialize() override;
		void Shutdown() override;
		void WaitForDevice() override;

		IDXGIFactory4* GetFactory() { return m_Factory; }
		ID3D12Device* GetDevice() { return m_Device; }

		ID3D12CommandQueue* GetGraphicsQueue() { return m_GraphicsQueue; }
		ID3D12CommandQueue* GetCopyQueue() { return m_CopyQueue; }
		ID3D12CommandQueue* GetComputeQueue() { return m_ComputeQueue; }

		static DirectXContext* Get() { return s_Instance; }
	private:
		ContextCreateInfo m_Info;

		ID3D12Debug* m_CpuDebug = nullptr;
		ID3D12Debug1* m_GpuDebug = nullptr;

		IDXGIFactory4* m_Factory = nullptr;
		ID3D12Device* m_Device = nullptr;

		ID3D12CommandQueue* m_GraphicsQueue = nullptr;
		ID3D12CommandQueue* m_CopyQueue = nullptr;
		ID3D12CommandQueue* m_ComputeQueue = nullptr;
	};
}