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
	
		void OnMaterialCreation(UUID Id);
		void OnSceneDestruction();
		void OnEntityCreation(Entity e);
		void OnEntityDestruction(Entity e);
	private:
		Ref<Shader> shader;
		Ref<Sampler> sampler;

		ResourceHeapHandle PerSceneHeap;
		UniformBufferHandle PerSceneUniform;
		
		std::unordered_map<UUID, ResourceHeapHandle> PerEntityHeap;
		std::unordered_map<UUID, UniformBufferHandle> PerEntityUniform;
		
		std::unordered_map<UUID, ResourceHeapHandle> PerMaterialHeap;
		std::unordered_map<UUID, CombinedImageSamplerHandle> PerMaterialSamplerImage;

		glm::vec4 materialBuffer;
		glm::mat4 matrixBuffer[2];
	};
}


