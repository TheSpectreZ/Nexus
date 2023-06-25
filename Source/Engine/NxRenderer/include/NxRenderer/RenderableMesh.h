#pragma once
#include "NxGraphics/Renderables.h"	

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	enum class MeshType
	{
		Static, Dynamic, Animated
	};

	struct RenderableMeshSpecification
	{
		MeshType Type;
		MeshSpecifications meshSpecs;
	};

	class RenderableMesh : Mesh
	{
	public:
		RenderableMesh(const RenderableMeshSpecification& specs);
		virtual ~RenderableMesh();

		Ref<Buffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<Buffer> GetIndexBuffer() { return m_IndexBuffer; }
	};
}