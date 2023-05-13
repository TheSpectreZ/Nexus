#pragma once
#include "Renderer/Framebuffer.h"
#include "Renderer/ShaderResource.h"
#include "glm/glm.hpp"

namespace Nexus
{
	class EditorViewport
	{
	public:
		~EditorViewport() = default;

		void Initialize();
		void SetContext(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex);
		void Render();

		glm::vec2 GetViewportSize();
	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Sampler> m_Sampler;
		std::vector<ImTextureID> m_TextureIDs;

		Extent m_Extent;
		ImVec2 m_Panelsize;
	};
}


