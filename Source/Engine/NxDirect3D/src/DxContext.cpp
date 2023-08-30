#include "NxDirect3D/DxContext.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

Nexus::DirectXContext* Nexus::DirectXContext::s_Instance = nullptr;

Nexus::DirectXContext::DirectXContext(const ContextCreateInfo& Info)
{
	m_Info = Info;
	s_Instance = this;
}

void Nexus::DirectXContext::Initialize()
{
	// Debug Controller
	{
		CHECK_CALL(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&m_CpuDebug));
		
		m_CpuDebug->EnableDebugLayer();

		CHECK_CALL(m_CpuDebug->QueryInterface(__uuidof(ID3D12Debug1), (void**)&m_GpuDebug));
		
		m_GpuDebug->SetEnableGPUBasedValidation(true);

		NEXUS_LOG("DIRECTX", "Debug Controller Created");
	}

	// Factory & Device
	{
		CHECK_CALL(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_Factory));
		
		Cptr<IDXGIAdapter1> adaptor;

		Cptr<IDXGIFactory6> factory6;
		m_Factory->QueryInterface(__uuidof(IDXGIFactory6), (void**)&factory6);

		uint32_t adaptorIndex = 0;

		while ( SUCCEEDED( factory6->EnumAdapterByGpuPreference( adaptorIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adaptor))) )
		{
			adaptorIndex++;

			DXGI_ADAPTER_DESC1 desc{};
			adaptor->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			auto hr = D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);

			if (SUCCEEDED(hr))
			{
				NEXUS_LOG("DirectX", "Physical Device: %s", reinterpret_cast<const char*>(desc.Description));
				break;
			}
		}

		CHECK_CALL(D3D12CreateDevice(adaptor.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_Device));
	
		m_Device->SetName(L"Nexus D3D Device");
		NEXUS_LOG("DirectX", "Device Created");
	}

	// Queues
	{
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CHECK_CALL(m_Device->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&m_GraphicsQueue));
		NEXUS_LOG("DirectX", "Graphics Queue Created");

		desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		CHECK_CALL(m_Device->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&m_CopyQueue));
		NEXUS_LOG("DirectX", "Graphics Queue Created");

		desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		CHECK_CALL(m_Device->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), (void**)&m_ComputeQueue));
		NEXUS_LOG("DirectX", "Graphics Queue Created");
	}
}

void Nexus::DirectXContext::Shutdown()
{
	// Queue
	m_GraphicsQueue->Release();
	m_ComputeQueue->Release();
	m_CopyQueue->Release();

	NEXUS_LOG("DirectX", "Queues Destroyed");

	// Device
	m_Device->Release();
	m_Factory->Release();

	NEXUS_LOG("DirectX", "Device Destroyed");

	// Debug Controller
	m_GpuDebug->Release();
	m_CpuDebug->Release();

	NEXUS_LOG("DirectX", "Debug Controller Destroyed");

}

void Nexus::DirectXContext::WaitForDevice()
{
}
