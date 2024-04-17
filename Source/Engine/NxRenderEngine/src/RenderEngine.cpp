#include "NxRenderEngine/RenderEngine.h"
#include "NxCore/Platform.h"
#include "NxCore/Logger.h"

#include "NxShaderCompiler/Compiler.h"

static std::string GetRenderBackendPath(Nexus::RenderEngineAPI api)
{
	switch (api)
	{
	case Nexus::RenderEngineAPI::Vulkan:
		return "NxVulkan.dll";
	default:
		return "";
	}
}

struct Nexus::RenderEngine::INTERNAL
{
	Platform::LibraryHANDLE* apiBackend;
	RenderEngineAPI eAPI;
};

bool Nexus::RenderEngine::Intialize(const RenderEngineSpecifications& specs)
{
	mInternal = new INTERNAL;
	mInternal->eAPI = specs.eAPI;

	mInternal->apiBackend = Platform::LoadSharedLibrary(GetRenderBackendPath(specs.eAPI));
	if (!mInternal->apiBackend)
		return false;

	typedef GraphicsDevice* (*Fn_CreateGraphicsDeviceFunc)();

	auto createGraphicsDevice = Platform::LoadFunc<Fn_CreateGraphicsDeviceFunc>(mInternal->apiBackend, "CreateGraphicsDevice");
	if (!createGraphicsDevice)
		return false;

	mGraphicsDevice = createGraphicsDevice();
	mGraphicsDevice->Initialize(GraphicsDeviceSpecifications
		{
			.pWindow = specs.pWindow,
			.hInstance = specs.hInstance
		});

	mSceneDrawer.Create(mGraphicsDevice, nullptr);

	NEXUS_LOG("Nexus", "RenderEngine Intialized");
	return true;
}

void Nexus::RenderEngine::Terminate()
{
	if (mGraphicsDevice)
	{
		mSceneDrawer.Destroy();
		
		mGraphicsDevice->Shutdown();
		delete mGraphicsDevice;
	}

	Platform::UnloadSharedLibrary(mInternal->apiBackend, GetRenderBackendPath(mInternal->eAPI));
	delete mInternal;

	NEXUS_LOG("Nexus", "RenderEngine Termianted");
}

void Nexus::RenderEngine::Flush()
{
	mGraphicsDevice->BeginFrame();
	
	mSceneDrawer.Draw();

	mGraphicsDevice->EndFrame( mSceneDrawer.GetPresentableFramebuffer(), 0);
	mGraphicsDevice->Flush();
}

void Nexus::RenderEngine::WaitForGPU()
{
	mGraphicsDevice->WaitForGPU();
}

void Nexus::RenderEngine::OnScreenResize(int width, int height)
{
	mGraphicsDevice->OnScreenResize(width, height);
}