#pragma once
#include "NxGraphics/Renderpass.h"
#include "NxDirect3D/DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXRenderpass : public Renderpass
	{
	public:
		DirectXRenderpass(const RenderpassSpecification& specs);
		~DirectXRenderpass() override;
	};
}
