#pragma once
#include "RenderTypes.h"
#include "Renderpass.h"

namespace Nexus
{
	enum FramebufferAttachmentType
	{
		Color, DepthStencil, PresentSrc, ShaderReadOnly_Color
	};

	struct FramebufferAttachmentDescription
	{
		FramebufferAttachmentType Type;
		bool multisampled;
	};

	struct FramebufferSpecification
	{
		Ref<Renderpass> renderpass;
		Extent extent;
		std::vector<FramebufferAttachmentDescription> attachments;
	};

	class Framebuffer
	{
	public:
		static Ref<Framebuffer> Create(const FramebufferSpecification& specs);

		Framebuffer() = default;
		virtual ~Framebuffer() = default;
	};
}

