#include "nxpch.h"
#include "AssetManager.h"
#include "Renderer/Texture.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize(bool LoadDefaultAssets)
{
	s_Instance = new AssetManager();
	
	// Defaults
	if(LoadDefaultAssets)
	{
		Ref<Asset> checker = Texture::Create("Resources/Textures/DefaultWhite.png");
		checker->m_Id = 0;
		Texture::s_LoadedTextures["Resources/Textures/DefaultWhite.png"] = 0;
		s_Instance->m_Assets[0] = checker;

		Ref<Asset> sampler = Sampler::Create(SamplerFilter::Nearest, SamplerFilter::Linear,
			SamplerWrapMode::Repeat, SamplerWrapMode::Repeat, SamplerWrapMode::Repeat);
		sampler->m_Id = 1;
		s_Instance->m_Assets[1] = sampler;
	
		auto samplerName = MakeSamplername(SamplerFilter::Nearest, SamplerFilter::Linear, SamplerWrapMode::Repeat, SamplerWrapMode::Repeat, SamplerWrapMode::Repeat);
		Sampler::s_LoadedSamplers[samplerName] = 1;
	}
}

void Nexus::AssetManager::Shutdown()
{
	delete s_Instance;
}