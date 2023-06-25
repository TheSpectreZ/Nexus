#pragma once
#include "NxGraphics/Shader.h"
#include "NxScene/Scene.h"
#include "NxScene/Entity.h"
#include "NxCore/UUID.h"

namespace Nexus
{
	class RenderableScene
	{
	public:
		RenderableScene(Ref<Scene> scene, Ref<Shader> shader);
		~RenderableScene();

		void InitResources();
		void UpdateResources();
		void DestroyResources();

		void OnMaterialCreation(UUID Id);
		void OnSceneDestruction();
		void OnEntityCreation(Entity e);
		void OnEntityDestruction(Entity e);
	private:
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
