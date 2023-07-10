#pragma once
#include "NxCore/UUID.h"
#include "NxGraphics/Renderables.h"	

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	struct NEXUS_RENDERER_API RenderableMaterial
	{
		RenderableMaterial() = default;
		~RenderableMaterial() = default;
	
		UUID m_Id;
		MaterialParameters m_Params;

		Ref<Texture> m_AlbedoMap;
		Ref<Texture> m_NormalMap;
		Ref<Texture> m_MetalicRoughnessMap;
	};

	class NEXUS_RENDERER_API MaterialTable
	{
	public:
		MaterialTable(const std::vector<RenderableMaterial> mats): m_Materials(mats) {}
		~MaterialTable() = default;

		std::vector<RenderableMaterial>& GetMaterials() { return m_Materials; }
	private:
		std::vector<RenderableMaterial> m_Materials;
	};

	class NEXUS_RENDERER_API RenderableMesh : public Mesh
	{
	public:
		RenderableMesh(const MeshSpecification& specs);
		~RenderableMesh() override;

		Ref<Buffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<Buffer> GetIndexBuffer() { return m_IndexBuffer; }

		Ref<MaterialTable> GetMaterialTable() { return m_MaterialTable; }
		bool SetMaterialTable(Ref<MaterialTable> matTable)
		{
			if (matTable->GetMaterials().size() != m_Submeshs.size())
				return false;

			m_MaterialTable = matTable;
			return true;
		}
	private:
		Ref<MaterialTable> m_MaterialTable;
	};

}