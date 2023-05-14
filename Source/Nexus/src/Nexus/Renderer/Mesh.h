#pragma once
#include "Assets/Asset.h"
#include "Buffer.h"

namespace Nexus
{
	struct SubMesh
	{
		Ref<StaticBuffer> vb, ib;
		UUID material = NullUUID;
		bool draw = true;
	};

	class StaticMesh : public Asset
	{
	public:
		static Ref<StaticMesh> Create(const std::string& filepath);
		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
	private:
		std::vector<SubMesh> m_SubMeshes;
	};
}
