#include "NxCore/Asset.h"
#include "NxCore/Logger.h"
#include <fstream>

std::string Nexus::Asset::GetTypeString()
{
	switch (m_Type)
	{
	case Nexus::AssetType::Mesh:
		return "Mesh-Asset";
	case Nexus::AssetType::Material:
		return "Material-Asset";
	case Nexus::AssetType::Texture:
		return "Texture-Asset";
	default:
		return "None";
	}
}

bool Nexus::BinarySerializer::Serialize(const AssetFilePath& path)
{
	std::ofstream stream(path, std::ios::binary);
	if (!stream.is_open())
		return false;

	BinaryBlob blob = OnSerialize();

	uint64_t size = blob.GetSize();
	stream.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
	stream.write(reinterpret_cast<const char*>(blob.GetMemory()), size * sizeof(uint8_t));

	stream.close();
	return true;
}

bool Nexus::BinarySerializer::Deserialize(const AssetFilePath& path)
{
	std::ifstream stream(path,std::ios::binary);
	if (!stream.is_open())
		return false;

	BinaryBlob blob;

	uint64_t size = 0;
	stream.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));

	blob.Resize(size);
	stream.read(reinterpret_cast<char*>(blob.GetMemory()), size * sizeof(uint8_t));

	OnDeserialize(blob);

	stream.close();
	return true;
}

bool Nexus::JsonSerializer::Serialize(const AssetFilePath& path)
{
	std::ofstream stream(path);
	if (!stream.is_open())
		return false;

	nlohmann::json Json = OnSerialize();
	std::string JsonDump = Json.dump(4);

	stream << JsonDump;
	stream.close();
	return true;
}

bool Nexus::JsonSerializer::Deserialize(const AssetFilePath& path)
{
	std::ifstream stream(path);
	if (!stream.is_open())
		return false;

	std::stringstream ss;
	ss << stream.rdbuf();

	std::string Json = ss.str();
	OnDeserialize(nlohmann::json::parse(Json));

	stream.close();
	return true;
}

void Nexus::BinarySerializer::BinaryBlob::Resize(uint64_t size)
{
	m_data.resize(size);
	m_offset = 0;
}

void Nexus::BinarySerializer::BinaryBlob::Write(void* data, uint64_t size)
{
	memcpy(m_data.data() + m_offset, data, size);
	m_offset += size;
}

void Nexus::BinarySerializer::BinaryBlob::Read(void* data, uint64_t size)
{
	memcpy(data, m_data.data() + m_offset, size);
	m_offset += size;
}
