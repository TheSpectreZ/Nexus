#pragma once
#include "GraphicsInterface.h"

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

		uint32_t MeshVerticesSize;
		void* MeshVerticesData;

		uint32_t MeshIndicesSize;
		void* MeshIndicesData;
	};

	class RenderableMesh
	{
	public:
		RenderableMesh(const RenderableMeshSpecification& specs);
		virtual ~RenderableMesh();

		Ref<Buffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<Buffer> GetIndexBuffer() { return m_IndexBuffer; }
	private:
		Ref<Buffer> m_VertexBuffer;
		Ref<Buffer> m_IndexBuffer;
	};
}