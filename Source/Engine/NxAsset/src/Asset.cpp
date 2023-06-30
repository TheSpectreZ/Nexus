#include "NxCore/UUID.h"

#include "NxAsset/Asset.h"
#include "NxAsset/glTFImporter.h"
#include "yaml-cpp/yaml.h"

#include <fstream>

static std::string AssetExtension = ".NxAsset";
static std::string BinExtension = ".NxBin";

namespace Nexus::Utils
{
	void Serialize(std::ofstream& outFile, const MeshSpecifications& obj)
	{
		uint32_t numElements = (uint32_t)obj.elements.size();
		outFile.write(reinterpret_cast<const char*>(&numElements), sizeof(uint32_t));

		for (const auto& element : obj.elements) 
		{
			uint32_t numVertices = (uint32_t)element.Vertices.size();

			outFile.write(reinterpret_cast<const char*>(&numVertices), sizeof(uint32_t));
			outFile.write(reinterpret_cast<const char*>(element.Vertices.data()), numVertices * sizeof(MeshVertex));

			uint32_t numIndices = (uint32_t)element.Indices.size();

			outFile.write(reinterpret_cast<const char*>(&numIndices), sizeof(uint32_t));
			outFile.write(reinterpret_cast<const char*>(element.Indices.data()), numIndices * sizeof(uint32_t));
		}

		uint32_t numMaterialIndices = (uint32_t)obj.materialIndices.size();
		outFile.write(reinterpret_cast<const char*>(&numMaterialIndices), sizeof(uint32_t));
		outFile.write(reinterpret_cast<const char*>(obj.materialIndices.data()), numMaterialIndices * sizeof(uint64_t));
	}

	void Deserialize(std::ifstream& inFile, MeshSpecifications& obj) 
	{
		uint32_t numElements = 0;
		inFile.read(reinterpret_cast<char*>(&numElements), sizeof(uint32_t));

		obj.elements.resize(numElements);

		for (auto& element : obj.elements) {
			uint32_t numVertices = 0;
			inFile.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));

			element.Vertices.resize(numVertices);
			inFile.read(reinterpret_cast<char*>(element.Vertices.data()), numVertices * sizeof(MeshVertex));

			uint32_t numIndices = 0;
			inFile.read(reinterpret_cast<char*>(&numIndices), sizeof(uint32_t));

			element.Indices.resize(numIndices);
			inFile.read(reinterpret_cast<char*>(element.Indices.data()), numIndices * sizeof(uint32_t));
		}

		uint32_t numMaterialIndices = 0;
		inFile.read(reinterpret_cast<char*>(&numMaterialIndices), sizeof(uint32_t));

		obj.materialIndices.resize(numMaterialIndices);
		inFile.read(reinterpret_cast<char*>(obj.materialIndices.data()), numMaterialIndices * sizeof(uint64_t));
	}
}

bool Nexus::MeshAsset::Import(const std::filesystem::path& sourcefilepath, const std::filesystem::path& AssetPath, const std::filesystem::path& BinPath)
{
	MeshSpecifications data;
	if (!Importer::glTF::Load(sourcefilepath, &data))
		return false;

	UUID Id;
	
	auto assetFile = AssetPath.string() + "\\" + sourcefilepath.filename().stem().string() + AssetExtension;
	auto binFile = BinPath.string() + "\\" + sourcefilepath.filename().stem().string() + BinExtension;

	// Meta
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Asset ID" << YAML::Value << Id.operator size_t();
			out << YAML::Key << "Asset Type" << YAML::Value << "Mesh Asset";
			{
				out << YAML::Key << "Asset Ref";
				out << YAML::BeginMap;
				out << YAML::Key << "Asset Source" << YAML::Value << sourcefilepath.string();
				out << YAML::Key << "Asset Bin" << YAML::Value << binFile;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		if (!std::filesystem::exists(AssetPath))
			std::filesystem::create_directories(AssetPath);

		std::ofstream fout(assetFile);
		fout << out.c_str();
	}

	// Bin
	{
		if (!std::filesystem::exists(BinPath))
			std::filesystem::create_directories(BinPath);

		std::ofstream fout;
		fout.open(binFile, std::ios::binary | std::ios::out);

		if (!fout.is_open())
			return false;
		
		Utils::Serialize(fout, data);
		fout.close();
	}

	return true;
}

bool Nexus::MeshAsset::Load(const std::filesystem::path& AssetPath)
{
	std::ifstream fin(AssetPath);

	std::stringstream stream;
	stream << fin.rdbuf();
	fin.close();

	YAML::Node data = YAML::Load(stream.str());
	
	if (!data["Asset ID"])
		return false;

	auto ref = data["Asset Ref"];

	if (!ref["Asset Bin"])
		return false;

	auto binpath = ref["Asset Bin"].as<std::string>();

	std::ifstream inFile(binpath, std::ios::binary);
	
	if (!inFile.is_open())
		return false;

	while (inFile.peek() != EOF) 
		Utils::Deserialize(inFile, m_Specs);
	
	inFile.close();
	
	return true;
}
