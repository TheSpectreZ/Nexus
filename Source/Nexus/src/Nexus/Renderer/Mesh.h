#pragma once
#include "Assets/Asset.h"
#include "Buffer.h"

namespace Nexus
{
	struct Material
	{
		UUID albedo;
	};

	struct SubMesh
	{
		Ref<StaticBuffer> vb, ib;
		uint32_t material;
		bool draw = true;
	};

	class StaticMesh : public Asset
	{
	public:
		static Ref<StaticMesh> Create(const std::string& filepath);
		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<Material> m_Materials;
	};
}
