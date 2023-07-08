#pragma once
#include "NxGraphics/Renderables.h"	

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	class NEXUS_RENDERER_API RenderableMesh : public Mesh
	{
	public:
		RenderableMesh(const MeshSpecification& specs);
		~RenderableMesh() override;

		Ref<Buffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<Buffer> GetIndexBuffer() { return m_IndexBuffer; }
	};
}