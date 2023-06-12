#pragma once
#include "TypeImpls.h"
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
		bool hdr;
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
		Framebuffer() = default;
		virtual ~Framebuffer() = default;
	};
}