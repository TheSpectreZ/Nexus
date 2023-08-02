#include "NxCore/Registry.h"
#include "nlohmann/json.hpp"
#include <fstream>

namespace stdfs = std::filesystem;

Nexus::AssetRegistry* Nexus::AssetRegistry::s_Instance = nullptr;

Nexus::AssetRegistry::AssetRegistry()
{
	s_Instance = this;
}

Nexus::AssetRegistry::~AssetRegistry()
{
	Serialize();
	s_Instance = nullptr;
}

void Nexus::AssetRegistry::SetProjectContext(const std::filesystem::path& projectDirectory)
{
	m_RegistryPath = projectDirectory.generic_string() + "/Config/Nexus.Reg";

	Deserialize();
	RegisterEngineAssets();
}

Nexus::UUID Nexus::AssetRegistry::Register(const std::filesystem::path& path)
{
	if(m_PathRegistry.contains(path))
		return m_PathRegistry[path];

	UUID Id;
	m_PathRegistry[path] = Id;
	m_IdRegistry[Id] = path;
	return Id;
}

std::filesystem::path Nexus::AssetRegistry::LookUp(UUID Id)
{
	if (m_IdRegistry.contains(Id))
		return m_IdRegistry[Id];

	return std::filesystem::path();
}

Nexus::UUID Nexus::AssetRegistry::LookUp(const std::filesystem::path& path)
{
	if (m_PathRegistry.contains(path))
		return m_PathRegistry[path];

	return UUID(true);
}

void Nexus::AssetRegistry::Serialize()
{
	if (!stdfs::exists(m_RegistryPath.parent_path()))
		stdfs::create_directories(m_RegistryPath.parent_path());

	nlohmann::json Json;

	for (auto& [k, v] : m_IdRegistry)
	{
		Json[v.generic_string()] = (uint64_t)k;
	}

	std::string JsonDump = Json.dump(4);
	
	std::ofstream stream(m_RegistryPath);
	stream << JsonDump;
}

void Nexus::AssetRegistry::Deserialize()
{
	if (!stdfs::exists(m_RegistryPath))
		return;

	std::ifstream stream(m_RegistryPath);
	
	std::stringstream ss;
	ss << stream.rdbuf();

	auto str = ss.str();
	if (str.empty())
		return;

	nlohmann::json Json = nlohmann::json::parse(str);

	for (auto& pair : Json.items())
	{
		stdfs::path path = std::filesystem::path( pair.key() );
		uint64_t Id = (uint64_t)pair.value();

		m_PathRegistry[path] = Id;
		m_IdRegistry[Id] = path;
	}
}

void Nexus::AssetRegistry::RegisterEngineAssets()
{

}

