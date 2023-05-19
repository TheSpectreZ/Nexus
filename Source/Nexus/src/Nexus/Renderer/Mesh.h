#pragma once
#include "Assets/Asset.h"
#include "Assets/Importer/glTFImporter.h"
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
		static std::unordered_map<std::string, UUID> s_LoadedMeshes;
	public:
		static Ref<StaticMesh> Create(const std::string& filepath, std::vector<UUID>* MaterialIds);
		static Ref<StaticMesh> Create(std::vector<Importer::glTF::Submesh>& submeshes, std::unordered_map<uint32_t, UUID>& MaterialIDs, const std::string& uniqueName);
		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
	private:
		std::vector<SubMesh> m_SubMeshes;
	};
}
