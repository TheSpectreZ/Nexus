#pragma once
#include "NxGraphics/Framebuffer.h"
#include "NxDirect3D/DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif

namespace Nexus
{
	class DirectXFramebuffer : public Framebuffer
	{
	public:
		DirectXFramebuffer(const FramebufferSpecification& specs);
		~DirectXFramebuffer() override;
	private:
		uint32_t m_IncrementalSize;
		ID3D12DescriptorHeap* m_Heap;

		struct Attachment
		{
			ID3D12Resource* resource;
			D3D12_RENDER_TARGET_VIEW_DESC view;
		};

		std::vector< std::vector< Attachment > > m_Attachments;
	};
}

