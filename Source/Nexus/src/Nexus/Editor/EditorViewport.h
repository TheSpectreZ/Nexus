#pragma once
#include "Renderer/Framebuffer.h"
#include "Renderer/ShaderResource.h"

namespace Nexus
{
	class EditorViewport
	{
	public:
		static Ref<EditorViewport> Create();

		EditorViewport() = default;
		virtual ~EditorViewport() = default;

		virtual void SetContext(Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) = 0;
		virtual void Render() = 0;
	};
}


