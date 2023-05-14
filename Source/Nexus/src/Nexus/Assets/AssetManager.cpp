#include "nxpch.h"
#include "AssetManager.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize()
{
	s_Instance = new AssetManager();
}

void Nexus::AssetManager::Shutdown()
{
	delete s_Instance;
}