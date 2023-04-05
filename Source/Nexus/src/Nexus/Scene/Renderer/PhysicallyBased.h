#pragma once
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Renderer/Shader.h"

namespace Nexus::SceneRenderer
{
	class PhysicallyBased
	{
	public:
		void Initialize(Ref<Shader> shader, Ref<Scene> scene,Camera* camera);
		void Terminate();
		void Render();
	private:
		Ref<Shader> m_Shader;
		Ref<Scene> m_Scene;
		Camera* m_Camera;

		uint32_t m_CallbackId;

		void BuildShaderData();

		void OnDeletionCallback(const Component::Identity& Identity);
	};
}