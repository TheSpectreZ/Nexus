#pragma once
#include "NxGraphics/CommandQueue.h"
#include "NxScene/Scene.h"
#include "NxScene/Entity.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif

namespace Nexus
{
	class NEXUS_RENDERER_API RenderableScene
	{
	public:
		RenderableScene(Ref<Scene> scene, Ref<Shader> shader);
		~RenderableScene();

		void Prepare();
		void Draw(Ref<CommandQueue> queue);
	private:
		void Initialize();
		void Destroy();

		Ref<Shader> m_Shader;
		Ref<Scene> m_Scene;
		
		ResourceHeapHandle PerSceneHeap;
		UniformBufferHandle PerSceneUniform0;
		UniformBufferHandle PerSceneUniform1;

		std::unordered_map<UUID, ResourceHeapHandle> PerEntityHeap;
		std::unordered_map<UUID, UniformBufferHandle> PerEntityUniform;

		std::unordered_map<UUID, ResourceHeapHandle> PerMaterialHeap;
		std::unordered_map<UUID, UniformBufferHandle> PerMaterialUniform;

		glm::mat4 matrixBuffer[2];
		glm::vec4 materialBuffer[3];

		static const uint32_t PointLightLimit = 10;

		struct SceneBuffer
		{
			glm::vec3 camPos; float pLightCount = 0.f;
			glm::vec4 lightDir;
			glm::vec4 lightCol;
			glm::vec4 null;

			struct pointLightBuffer
			{
				glm::vec3 pos; float size;
				glm::vec3 col; float intensity;
				glm::vec3 null; float faloff;
				glm::vec4 n;
			};

			pointLightBuffer pointLights[PointLightLimit];
		} m_SceneBuffer;
	};
}
