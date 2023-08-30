#pragma once
#include "NxCore/Window.h"
#include "NxGraphics/Swapchain.h"
#include "NxDirect3D/DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif // NEXUS_DIRECTX_SHARED_BUILD

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXSwapchain : public Swapchain
	{
		static DirectXSwapchain* s_Instance;
	public:
		DirectXSwapchain(Window* window);
		~DirectXSwapchain() override;

		static DirectXSwapchain* Get() { return s_Instance; }

		void Initialize() override;
		void Shutdown() override;

		IDXGISwapChain3* GetHandle() { return m_Swapchain; }
		uint32_t GetImageCount() override { return 0; }
		Extent GetExtent() override;
		DXGI_FORMAT GetFormat() { return m_Format; }
	private:
		IDXGISwapChain3* m_Swapchain = nullptr;

		Window* m_window;
		uint32_t m_MinImageCount;
		DXGI_FORMAT m_Format;
	};
}
