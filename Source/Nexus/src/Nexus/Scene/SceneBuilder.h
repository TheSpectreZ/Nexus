#pragma once
#include "Renderer/Shader.h"
#include "Scene.h"
#include "Camera.h"

namespace Nexus
{
	// Assuming that This Builds data for the PBR Shader

	//  [ToDo] Make Buffers for each swapchain image

	class SceneBuildData
	{
		friend class SceneRenderer;
	public:
		static Ref<SceneBuildData> Build(Ref<Scene> scene, Ref<Shader> shader);
		void Update(Ref<Scene> scene, Camera camera);
		void Destroy();
	
		void OnSceneDestruction();
		void OnEntityCreation(Entity e);
		void OnEntityDestruction(Entity e);
	private:
		Ref<Shader> shader;

		ResourceHeapHandle PerSceneHeap;
		UniformBufferHandle PerSceneUniform;

		std::unordered_map<UUID, ResourceHeapHandle> PerEntityHeap;
		std::unordered_map<UUID, UniformBufferHandle> PerEntityUniform;

		glm::mat4 matrixBuffer[2];
	};
}


