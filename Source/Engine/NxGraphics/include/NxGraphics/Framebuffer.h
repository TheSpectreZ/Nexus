#pragma once
#include "Specification.h"

namespace Nexus
{
	struct FramebufferAttachment
	{
		ImageFormat format = ImageFormat::eUndefined;
		ImageLoadOp loadOp = ImageLoadOp::eLoad;
		ImageStoreOp storeOp = ImageStoreOp::eStore;

		ImageClearValue clearValue = ImageClearColorValue(0.f, 0.f, 0.f, 1.f);

		bool bMultiSampled = false;
		bool bResolve = false;
	};

	struct FramebufferSpecification
	{
		std::vector< FramebufferAttachment > colorAttachments;
		FramebufferAttachment depthStencilAttachment;
		Extent extent;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {};

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void Resize(int width, int height) = 0;
	};

	typedef std::pair<Framebuffer*, uint32_t> Presentable;
}