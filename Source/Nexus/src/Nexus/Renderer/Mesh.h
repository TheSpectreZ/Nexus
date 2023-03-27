#pragma once
#include "Buffer.h"

namespace Nexus
{
	class StaticMesh
	{
	public:
		static Ref<StaticMesh> LoadWithAssimp(const char* Filepath);

		Ref<StaticBuffer> GetVertexBuffer() { return m_Vb; }
		Ref<StaticBuffer> GetIndexBuffer() { return m_Ib; }
	private:
		Ref<StaticBuffer> m_Vb;
		Ref<StaticBuffer> m_Ib;
	};
}
