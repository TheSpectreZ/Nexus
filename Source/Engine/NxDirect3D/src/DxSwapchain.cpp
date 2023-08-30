#include "NxDirect3D/DxSwapchain.h"
#include "NxDirect3D/DxContext.h"

Nexus::DirectXSwapchain* Nexus::DirectXSwapchain::s_Instance = nullptr;

Nexus::DirectXSwapchain::DirectXSwapchain(Window* window)
{
	s_Instance = this;
	m_window = window;

	m_MinImageCount = 2;
}

Nexus::DirectXSwapchain::~DirectXSwapchain()
{
	s_Instance = nullptr;
}

void Nexus::DirectXSwapchain::Initialize()
{
	auto context = DirectXContext::Get();

	// Swapchain
	{
		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = m_MinImageCount;
		desc.BufferDesc.Width = m_window->width;
		desc.BufferDesc.Height = m_window->height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.OutputWindow = (HWND)m_window->nativeHandle;
		desc.SampleDesc.Count = 1;
		desc.Windowed = TRUE;
		
		IDXGISwapChain* swapchain = nullptr;

		auto factory = context->GetFactory();
		
		CHECK_CALL(factory->CreateSwapChain(context->GetGraphicsQueue(), &desc, &swapchain));
		CHECK_CALL(swapchain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_Swapchain));

		factory->MakeWindowAssociation((HWND)m_window->nativeHandle, DXGI_MWA_NO_ALT_ENTER);
		
		m_Format = desc.BufferDesc.Format;

		NEXUS_LOG("DirectX", "Swapchain Created");
	}
}

void Nexus::DirectXSwapchain::Shutdown()
{
	m_Swapchain->Release();
	NEXUS_LOG("DirectX", "Swapchain Destroyed");
}

Nexus::Extent Nexus::DirectXSwapchain::GetExtent()
{
	return Extent();
}
