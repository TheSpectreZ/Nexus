#include "NxRenderEngine/RenderEngine.h"
#include "NxCore/Platform.h"
#include "NxCore/Logger.h"

#include "NxRenderEngine/SceneDrawer.h"

static std::string GetRenderBackendPath(Nexus::GraphicsAPI api)
{
	switch (api)
	{
	case Nexus::GraphicsAPI::eVulkan:
		return "NxVulkan.dll";
	default:
		return "";
	}
}

struct Nexus::RenderEngine::INTERNAL
{
	Platform::LibraryHANDLE* apiBackend;
	GraphicsAPI eAPI;
};

bool Nexus::RenderEngine::Initialize(const RenderEngineSpecifications& specs)
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

	mDevice = createGraphicsDevice();
	mDevice->Initialize(GraphicsDeviceSpecifications
		{
			.pWindow = specs.pWindow,
			.hInstance = specs.hInstance
		});
	
	NEXUS_LOG("Nexus", "RenderEngine Intialized");
	return true;
}

void Nexus::RenderEngine::Terminate()
{
	if (mDevice)
	{
		mDevice->Shutdown();
		delete mDevice;
	}

	Platform::UnloadSharedLibrary(mInternal->apiBackend, GetRenderBackendPath(mInternal->eAPI));
	delete mInternal;

	NEXUS_LOG("Nexus", "RenderEngine Termianted");
}

void Nexus::RenderEngine::Flush()
{
	mDevice->Flush();
}

void Nexus::RenderEngine::WaitForGPU()
{
	mDevice->WaitForGPU();
}

void Nexus::RenderEngine::OnScreenResize(int width, int height)
{
	mDevice->OnScreenResize(width, height);
}