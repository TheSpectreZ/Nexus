#include "nxpch.h"
#include "AssetManager.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize()
{
	s_Instance = new AssetManager();
}

void Nexus::AssetManager::Shutdown()
{
	s_Instance->m_Meshes.clear();

	delete s_Instance;
}

Nexus::AssetHandle Nexus::AssetManager::CreateAssetHandle()
{
	static AssetHandle handle = 0;
	return handle++;
}

#define GET_ASSET(Type,Member) template<>\
Nexus::Ref<Type> Nexus::AssetManager::Get<Type>(AssetHandle handle)\
{\
	return s_Instance->Member[handle];\
}\

#define HAS_ASSET(Type,Member) template<>\
bool Nexus::AssetManager::Has<Type>(AssetHandle handle)\
{\
	return s_Instance->Member.contains(handle);\
}\

#define REMOVE_ASSET(Type,Member) template<>\
void Nexus::AssetManager::Remove<Type>(AssetHandle handle)\
{\
	s_Instance->Member.erase(handle);\
}\


template<>
Nexus::AssetHandle Nexus::AssetManager::LoadFromFile<Nexus::StaticMesh>(const std::filesystem::path& path)
{
	if (path.extension().string() != ".fbx")
		return UINT64_MAX;

	AssetHandle handle = CreateAssetHandle();

	s_Instance->m_Meshes[handle] = StaticMesh::LoadWithAssimp(path.string().c_str());
	return handle;
}

GET_ASSET(Nexus::StaticMesh,m_Meshes)
HAS_ASSET(Nexus::StaticMesh,m_Meshes)
REMOVE_ASSET(Nexus::StaticMesh,m_Meshes)