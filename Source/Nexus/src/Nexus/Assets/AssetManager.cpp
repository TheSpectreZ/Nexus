#include "nxpch.h"
#include "AssetManager.h"
#include "Renderer/Texture.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize()
{
	s_Instance = new AssetManager();
	
	// Defaults
	{
		Ref<Asset> checker = Texture::Create("Resources/Textures/DefaultWhite.png");
		checker->m_Id = 0;
		
		s_Instance->m_Assets[0] = checker;

		Ref<Asset> sampler = Sampler::Create(SamplerFilter::Nearest, SamplerFilter::Linear,
			SamplerWrapMode::Repeat, SamplerWrapMode::Repeat, SamplerWrapMode::Repeat);
		sampler->m_Id = 1;
		s_Instance->m_Assets[1] = sampler;
	}
}

void Nexus::AssetManager::Shutdown()
{
	delete s_Instance;
}