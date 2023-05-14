#pragma once
#include "Buffer.h"

namespace Nexus
{
	struct SubMesh
	{
		Ref<StaticBuffer> vb, ib;
		bool draw = false;
	};

	class StaticMesh
	{
	public:
		static Ref<StaticMesh> LoadWithAssimp(const char* Filepath);

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
	private:
		std::vector<SubMesh> m_SubMeshes;
	};
}
