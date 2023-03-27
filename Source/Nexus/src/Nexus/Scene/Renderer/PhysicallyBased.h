#pragma once
#include "Scene/Scene.h"
#include "Renderer/Shader.h"

namespace Nexus::SceneRenderer
{
	class PhysicallyBased
	{
	public:
		void Setup(Ref<Shader> shader, Ref<Scene> scene);
		void Terminate();
		void Render();
	private:
		Ref<Shader> m_Shader;
		Ref<Scene> m_Scene;
	};
}