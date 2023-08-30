#pragma once
#include "NxGraphics/Buffer.h"
#include "DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXBuffer : public Buffer
	{
	public:
		DirectXBuffer(const BufferSpecification& specs);
		~DirectXBuffer() override;

		void Update(void* data) override;
		
		struct BufferView {};

		struct VertexBufferView : public BufferView	{ D3D12_VERTEX_BUFFER_VIEW handle; };
		struct IndexBufferView : public BufferView { D3D12_INDEX_BUFFER_VIEW handle; };
		struct UniformBufferView : public BufferView { D3D12_CONSTANT_BUFFER_VIEW_DESC handle; };

		ID3D12Resource* Get() { return m_Buffer; }
		BufferView* GetView() { return m_View; }
		uint32_t GetSize() { return m_Size; }
		uint32_t GetStride() { return m_Stride; }
	private:
		BufferView* m_View;
		BufferType m_Type; uint32_t m_Size; uint32_t m_Stride;
		ID3D12Resource* m_Buffer;
	};
}