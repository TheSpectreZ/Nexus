#include "NxRenderEngine/SceneDrawer.h"

#include "NxShaderCompiler/Compiler.h"

Nexus::SceneDrawer::SceneDrawer(GraphicsDevice* pDevice, Scene* pScene, Resolution resolution)
	:mDevice(pDevice), mScene(pScene)
{
	ShaderSpecification shaderSpecs;
	ShaderCompiler::CompileFromFile("Assets/Shaders/FullscreenVS.glsl", ShaderType::eVertex, &shaderSpecs);

	mTestShaderObject = pDevice->CreateShaderObject(shaderSpecs);

	ResolutionChangeEvent res(resolution);
	CreateSceneResources(res.width, res.height);
}

void Nexus::SceneDrawer::Draw()
{
	mTestFramebuffer->Begin();
	mTestFramebuffer->End();
}

void Nexus::SceneDrawer::Resize(uint32_t width, uint32_t height)
{
	mTestFramebuffer->Resize(width, height);
}

Nexus::SceneDrawer::~SceneDrawer()
{
	DestroySceneResources();
	mTestShaderObject.reset();
}

Nexus::Presentable Nexus::SceneDrawer::getPresentable()
{
	return { mTestFramebuffer.get(), 0 };
}

void Nexus::SceneDrawer::CreateSceneResources(int width, int height)
{
	FramebufferAttachment colorAttachment{
		.format = ImageFormat::eRGBA8_SFLOAT,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearColorValue{ 0.9f, 0.3f, 0.5f, 1.f }
	};
	
	FramebufferAttachment colorAttachment2{
		.format = ImageFormat::eRGBA8_SFLOAT,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearColorValue{ 0.5f, 0.9f, 0.4f, 1.f }
	};

	FramebufferAttachment depthAttachment{
		.format = ImageFormat::eD32_SFLOAT,
		.loadOp = ImageLoadOp::eClear,
		.clearValue = ImageClearDepthStencilValue{ 1.f, 0 }
	};

	FramebufferSpecification specs{};
	specs.extent.width = width;
	specs.extent.height = height;
	specs.colorAttachments = { colorAttachment, colorAttachment2 };
	specs.depthStencilAttachment = depthAttachment;

	mTestFramebuffer = mDevice->CreateFramebuffer(specs);
}

void Nexus::SceneDrawer::DestroySceneResources()
{
	mTestFramebuffer.reset();
}
