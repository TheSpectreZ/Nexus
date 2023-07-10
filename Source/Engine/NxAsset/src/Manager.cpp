#include "NxAsset/Manager.h"

Nexus::Module::AssetManager* Nexus::Module::AssetManager::s_Instance = nullptr;

void Nexus::Module::AssetManager::Initialize()
{
	s_Instance = new AssetManager;
}

void Nexus::Module::AssetManager::Shutdown()
{
	delete s_Instance;
}

#define IMPORT_SWITCH(type,cls,src,Ast,Bin) case Nexus::type:\
{\
	Ref<Nexus::Asset> asset = CreateRef<cls>();\
	if(asset->Import(src,Ast,Bin))\
		return true;\
}

#define LOAD_SWITCH(type,cls) case Nexus::type:\
{\
	auto a = CreateRef<cls>();\
	if(!a->Load(Asset))\
		return res;\
	m_Assets[res.id] = a;\
	break;\
}\
	
bool Nexus::Module::AssetManager::Import(AssetType type, const AssetFilePath& Source, const AssetFilePath& Asset, const AssetFilePath& Bin)
{
	switch (type)
	{
	IMPORT_SWITCH(AssetType::Mesh,MeshAsset,Source, Asset, Bin)
	IMPORT_SWITCH(AssetType::Texture,TextureAsset,Source,Asset,Bin)
	IMPORT_SWITCH(AssetType::MaterialTable,MaterialTableAsset,Source,Asset,Bin)
	default:
		return false;
	}

	return false;
}

Nexus::Module::AssetLoadResult Nexus::Module::AssetManager::Load(AssetType type, const AssetFilePath& Asset)
{
	AssetLoadResult res;

	if (!Utils::GetUUIDFromAssetFile(Asset, res.id, nullptr))
		return res;

	if (m_Assets.contains(res.id))
	{
		auto Type = m_Assets[res.id]->GetType();
		if (Type == type)
		{
			res.success = true;
			res.asset = m_Assets[res.id];
			return res;
		}
	}

	switch (type)
	{
	LOAD_SWITCH(AssetType::Mesh, MeshAsset)
	LOAD_SWITCH(AssetType::Texture, TextureAsset)
	LOAD_SWITCH(AssetType::MaterialTable, MaterialTableAsset)
	default:
		return res;
	}

	res.success = true;
	res.asset = m_Assets[res.id];
	return res;
}