#pragma once
#include "Renderer/Vertex.h"
#include "Renderer/Buffer.h"

#include "glm/glm.hpp"

namespace Nexus
{
	class VulkanCommand;

	class StaticMesh
	{
		friend class VulkanCommand;
	public:
		static Ref<StaticMesh> LoadWithAssimp(const char* Filepath);
		
	private:
		Ref<StaticBuffer> m_Vb;
		Ref<StaticBuffer> m_Ib;
	};
}
