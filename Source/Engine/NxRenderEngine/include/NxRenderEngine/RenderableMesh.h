#pragma once
#include "NxCore/UUID.h"
#include "NxCore/Object.h"
#include "NxGraphics/Buffer.h"
#include "NxGraphics/Meshing.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	class NEXUS_RENDERER_API RenderableMesh : public BaseAsset
	{
	public:
		RenderableMesh(const Meshing::Mesh& mesh);
		~RenderableMesh();

		Ref<Buffer> GetVertexBuffer() { return m_VB; }
		Ref<Buffer> GetIndexBuffer() { return m_IB; }

		std::vector<Meshing::Submesh>& GetSubmeshes() { return m_Submeshes; }

		std::string GetAssetTypeString() override { return "MeshAsset"; }
		uint8_t GetAssetTypeIndex() override { return 1; }
	private:
		Ref<Buffer> m_VB;
		Ref<Buffer> m_IB;

		std::vector<Meshing::Submesh> m_Submeshes;
	};

}