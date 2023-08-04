#pragma once
#include <vector>
#include "NxScene/Camera.h"
#include "GraphicsInterface.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define	NEXUS_RENDER_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_RENDER_ENGINE_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	class NEXUS_RENDER_ENGINE_API BatchRenderer
	{
		static const uint32_t s_VertexCount = 1024 * 2;
		static BatchRenderer* s_Instance;
	public:
		static BatchRenderer* Get() { return s_Instance; }
		static void Initialize(Ref<Renderpass> renderpass);
		static void Shutdown();

		void DrawLine(const glm::vec3& a, const glm::vec3& b);
		//void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		void Flush(Ref<CommandQueue> queue, Camera* sceneCamera);
	private:
		void InitImpl(Ref<Renderpass> renderpass);
		void ShutImpl();
		void FlushToBuffer();

		Ref<Shader> m_Shader;
		Ref<Pipeline> m_pipeline;

		ResourceHeapHandle m_HeapHandle;
		UniformBufferHandle m_UniformHandle;

		BufferSpecification bspecs{};

		uint32_t m_CurrentBufferIndex = 0;
		uint32_t m_CurrentVertexIndex = 0;

		std::vector< Ref<Buffer> > m_VertexBuffer;
		std::vector< glm::vec3 > m_VertexData;
	};
}
