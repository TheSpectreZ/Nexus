#pragma once
#include "RenderTypes.h"
#include "Renderpass.h"

namespace Nexus
{
	enum FramebufferAttachmentType
	{
		Color, DepthStencil, PresentSrc
	};

	struct FramebufferAttachmentDescription
	{
		FramebufferAttachmentType Type;
		Extent extent;
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

