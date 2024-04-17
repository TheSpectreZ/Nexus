#include "NxRenderEngine/SceneDrawer.h"

void Nexus::SceneDrawer::Create(GraphicsDevice* pDevice, const Scene* pScene)
{
	FramebufferAttachment colorAttachment{
		.format = ImageFormat::eRGBA8_srgb,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearColorValue{ 0.9f, 0.5f, 0.8f, 1.f }
	};
	
	FramebufferAttachment depthAttachment{
		.format = ImageFormat::eD32_sfloat,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearDepthStencilValue{ 1.f, 0 }
	};

	FramebufferSpecification specs{};
	specs.extent = pDevice->getDeviceExtent();
	specs.colorAttachments = { colorAttachment };
	specs.depthStencilAttachment = depthAttachment;

	mTestFramebuffer = pDevice->CreateFramebuffer(specs);
}

void Nexus::SceneDrawer::Draw()
{
	mTestFramebuffer->Begin();
	mTestFramebuffer->End();
}

void Nexus::SceneDrawer::Destroy()
{
	mTestFramebuffer.reset();
}
