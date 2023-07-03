#pragma once
#include "NxRenderer/GraphicsInterface.h"
#include "glm/glm.hpp"
#include "imgui.h"

namespace NexusEd
{
	class Viewport
	{
	public:
		~Viewport() = default;

		void Initialize();
		void SetContext(Nexus::Ref<Nexus::Framebuffer> framebuffer, uint32_t attachmentIndex);
		void Render();

		glm::vec2 GetViewportSize();
	private:
		Nexus::Ref<Nexus::Framebuffer> m_Framebuffer;
		Nexus::Ref<Nexus::Sampler> m_Sampler;
		std::vector<ImTextureID> m_TextureIDs;

		Nexus::Extent m_Extent;
		ImVec2 m_Panelsize;
	};
}


