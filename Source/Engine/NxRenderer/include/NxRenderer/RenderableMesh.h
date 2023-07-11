#pragma once
#include "NxCore/UUID.h"
#include "NxGraphics/Buffer.h"
#include "NxGraphics/Meshing.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	
	class NEXUS_RENDERER_API RenderableMesh
	{
		struct Mesh
		{
			Ref<Buffer> vb, ib;
			std::vector<Meshing::Submesh> sb;
		};

	public:
		RenderableMesh(const std::vector<Meshing::Mesh>& meshes);
		~RenderableMesh();

		uint32_t GetMeshCount() { return (uint32_t)m_Meshes.size(); }
		
		Ref<Buffer> GetVertexBuffer(uint32_t index) { return m_Meshes[index].vb; }
		Ref<Buffer> GetIndexBuffer(uint32_t index) { return m_Meshes[index].ib; }

		std::vector<Meshing::Submesh> GetSubmeshes(uint32_t index) { return m_Meshes[index].sb; }
	private:
		std::vector<Mesh> m_Meshes;
	};

}