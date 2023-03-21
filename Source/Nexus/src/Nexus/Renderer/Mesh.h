#pragma once
#include "Vertex.h"
#include "Buffer.h"

#include "glm/glm.hpp"

namespace Nexus
{
	class VulkanCommand;

	class StaticMesh
	{
		friend class VulkanCommand;
	public:
		static Ref<StaticMesh> LoadWithAssimp(const char* Filepath);
		static Ref<StaticMesh> LoadPlane();

		void Destroy();
	private:
		Ref<StaticBuffer> m_Vb;
		Ref<StaticBuffer> m_Ib;

		std::vector<StaticMeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
