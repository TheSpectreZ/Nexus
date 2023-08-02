#include "NxAsset/Manager.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize()
{
	s_Instance = new AssetManager;
}

void Nexus::AssetManager::Shutdown()
{
	RenderableMesh::ClearPool();
	RenderableMaterial::ClearPool();
	RenderableTexture::ClearPool();

	delete s_Instance;
}
