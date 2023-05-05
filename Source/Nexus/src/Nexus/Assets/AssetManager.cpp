#include "nxpch.h"
#include "AssetManager.h"

Nexus::AssetManager* Nexus::AssetManager::s_Instance = nullptr;

void Nexus::AssetManager::Initialize()
{
	s_Instance = new AssetManager();
}

void Nexus::AssetManager::Shutdown()
{
	s_Instance->m_StaticMeshes.clear();
	delete s_Instance;
}

Nexus::AssetHandle Nexus::AssetManager::CreateAssetHandle()
{
	static AssetHandle handle = 0;
	return handle++;
}

#define GET_ASSET(Type,Member) template<>\
Type& Nexus::AssetManager::Get<Type>(AssetHandle handle)\
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
Nexus::AssetHandle Nexus::AssetManager::LoadFromFile<Nexus::StaticMeshAsset>(const std::filesystem::path& path)
{
	if (path.extension().string() != ".fbx")
		return UINT64_MAX;

	std::string p = path.string();
	if (s_Instance->m_AssetHandleCache.contains(p))
	{
		return s_Instance->m_AssetHandleCache[p];
	}

	AssetHandle handle = CreateAssetHandle();

	s_Instance->m_StaticMeshes[handle].Mesh = StaticMesh::LoadWithAssimp(path.string().c_str());
	s_Instance->m_StaticMeshes[handle].Name = path.filename().string();
	s_Instance->m_StaticMeshes[handle].Path = path;

	s_Instance->m_AssetHandleCache[p] = handle;

	return handle;
}


GET_ASSET(Nexus::StaticMeshAsset, m_StaticMeshes)
HAS_ASSET(Nexus::StaticMeshAsset, m_StaticMeshes)
REMOVE_ASSET(Nexus::StaticMeshAsset, m_StaticMeshes)