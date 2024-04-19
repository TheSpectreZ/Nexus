#include "NxRenderEngine/SceneDrawer.h"

#include "NxShaderCompiler/Compiler.h"

Nexus::SceneDrawer::SceneDrawer(GraphicsDevice* pDevice)
{
	FramebufferAttachment colorAttachment{
		.format = ImageFormat::eRGBA8_SFLOAT,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearColorValue{ 0.9f, 0.5f, 0.8f, 1.f }
	};
	
	FramebufferAttachment depthAttachment{
		.format = ImageFormat::eD32_SFLOAT,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearDepthStencilValue{ 1.f, 0 }
	};

	FramebufferSpecification specs{};
	specs.extent = pDevice->getDeviceExtent();
	specs.colorAttachments = { colorAttachment };
	specs.depthStencilAttachment = depthAttachment;

	mTestFramebuffer = pDevice->CreateFramebuffer(specs);

	ShaderSpecification shaderSpecs;
	ShaderCompiler::CompileFromFile("Assets/Shaders/FullscreenVS.glsl", ShaderType::eVertex, &shaderSpecs);

	mTestShaderObject = pDevice->CreateShaderObject(shaderSpecs);
}

void Nexus::SceneDrawer::Draw()
{
	mTestFramebuffer->Begin();
	mTestFramebuffer->End();
}

void Nexus::SceneDrawer::OnWindowResize(int width, int height)
{

}

Nexus::SceneDrawer::~SceneDrawer()
{
	mTestShaderObject.reset();
	mTestFramebuffer.reset();
}
