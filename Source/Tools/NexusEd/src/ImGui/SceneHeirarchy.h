#pragma once
#include "NxScene/Entity.h"

namespace NexusEd
{
	class SceneHeirarchy
	{
	public:
		void SetContext(Nexus::Ref<Nexus::Scene> scene);
		void Render();
	private:
		Nexus::Ref<Nexus::Scene> m_Scene;
		entt::entity m_SelectedEntity;

		void DrawEntityNode(entt::entity e);
		void DrawComponents(entt::entity e);
	};

	namespace ImGuiUtils
	{
		bool DrawVec3Control(const char* label, glm::vec3& vector, float reset = 0.f, float columnWidth = 100.f);
	}
}
