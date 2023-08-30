#pragma once
#include "NxGraphics/Shader.h"
#include "DxAssert.h"

#ifdef NEXUS_DIRECTX_SHARED_BUILD
#define NEXUS_DIRECTX_API __declspec(dllexport)
#else
#define NEXUS_DIRECTX_API __declspec(dllimport)
#endif // NEXUS_DIRECTX_SHARED_BUILD

namespace Nexus
{
	class NEXUS_DIRECTX_API DirectXShader : public Shader
	{
	public:
		DirectXShader(const ShaderSpecification& specs);
		~DirectXShader() override;

		void AllocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void DeallocateShaderResourceHeap(ResourceHeapHandle handle) override;
		void GetShaderResourceHeapLayoutBinding(ShaderResouceHeapLayoutBinding*& heap, uint32_t set, uint32_t binding) override;

		void BindUniformWithResourceHeap(ResourceHeapHandle heapHandle, uint32_t binding, Ref<Buffer> buffer) override;
		void BindTextureWithResourceHeap(ResourceHeapHandle heapHandle, ImageHandle texture) override;
	private:

	};
}