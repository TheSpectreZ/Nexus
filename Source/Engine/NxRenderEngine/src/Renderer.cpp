#include "NxRenderEngine/Renderer.h"
#include "NxCore/Platform.h"
#include "NxCore/Logger.h"

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
	Platform::LibraryHANDLE* apiLibrary;
	RenderEngineAPI eAPI;
};

bool Nexus::RenderEngine::Intialize(const RenderEngineSpecifications& specs)
{
	mInternal = new INTERNAL;
	mInternal->eAPI = specs.eAPI;

	mInternal->apiLibrary = Platform::LoadSharedLibrary(GetRenderBackendPath(specs.eAPI));
	if (!mInternal->apiLibrary)
		return false;

	typedef GraphicsDevice* (*Fn_CreateGraphicsDeviceFunc)();

	auto createGraphicsDevice = Platform::LoadFunc<Fn_CreateGraphicsDeviceFunc>(mInternal->apiLibrary, "CreateGraphicsDevice");
	if (!createGraphicsDevice)
		return false;

	mGraphicsDevice = createGraphicsDevice();
	mGraphicsDevice->Initialize(GraphicsDeviceSpecifications
		{
			.pWindow = specs.pWindow,
			.hInstance = specs.hInstance
		});

	NEXUS_LOG("Nexus", "RenderEngine Intialized");
	return true;
}

void Nexus::RenderEngine::Terminate()
{
	if (mGraphicsDevice)
	{
		mGraphicsDevice->Shutdown();
		delete mGraphicsDevice;
	}

	Platform::UnloadSharedLibrary(mInternal->apiLibrary, GetRenderBackendPath(mInternal->eAPI));
	delete mInternal;

	NEXUS_LOG("Nexus", "RenderEngine Termianted");
}

void Nexus::RenderEngine::Flush()
{
	mGraphicsDevice->BeginFrame();
	
	mGraphicsDevice->EndFrame();
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