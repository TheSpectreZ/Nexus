#pragma once
#include "NxGraphics/CommandQueue.h"
#include "NxScene/Scene.h"
#include "NxScene/Entity.h"
#include "RenderableMesh.h"

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
		RenderableScene(Ref<Shader> pbrShader, Ref<Shader> skyboxShader);
		~RenderableScene();

		void Prepare(Ref<Scene> scene);
		void DrawSkybox(Ref<CommandQueue> queue);
		void DrawScene(Ref<CommandQueue> queue,Ref<Scene> scene);
	private:
		void Initialize();
		void Destroy();

		void CreateEntityResource(UUID Id);
		void CreateMaterialResource(UUID Id);
		
		Ref<Shader> m_pbrShader, m_skyBoxShader;
		
		ResourceHeapHandle SkyBoxHeap;
		
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

		struct MaterialBuffer
		{
			glm::vec4 albedo;
			float roughness, metalness;
			float useNormal;

			float nul;
		} m_MaterialBuffer;

		struct SceneBuffer
		{
			glm::vec3 lightDir;
			float pLightCount = 0.f;
			glm::vec4 lightCol;
			int useIBL = -1;
			glm::vec3 null;

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
