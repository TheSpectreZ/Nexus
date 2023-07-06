#include "yaml-cpp/yaml.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "NxCore/UUID.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxAsset/Asset.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#include "tiny_gltf.h"

static std::string AssetExtension = ".NxAsset";
static std::string BinExtension = ".NxBin";

namespace Nexus::Utils
{
	std::string GetStringFromType(AssetType type)
	{
		switch (type)
		{
		case Nexus::AssetType::Mesh:
			return "MeshAsset";
		case Nexus::AssetType::Texture:
			return "TextureAsset";
		default:
			return "Invalid";
		}
	}

	AssetType GetTypeFromString(const std::string& str)
	{
		if (str == "MeshAsset")
			return AssetType::Mesh;
		else if (str == "TextureAsset")
			return AssetType::Texture;
		else
			return AssetType::None;
	}

	void SerializeMeshAsset(std::ofstream& outFile, const MeshSpecification& obj)
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

	void DeserializeMeshAsset(std::ifstream& inFile, MeshSpecification& obj) 
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

	void SerializeTextureAsset(std::ofstream& outFile, const TextureSpecification& obj)
	{
		outFile.write(reinterpret_cast<const char*>(&obj.extent), sizeof(Extent));
		outFile.write(reinterpret_cast<const char*>(obj.pixeldata), sizeof(uint8_t) * obj.extent.width * obj.extent.height * 4);
	}

	void DeserializeTextureAsset(std::ifstream& inFile, TextureSpecification& obj)
	{
		inFile.read(reinterpret_cast<char*>(&obj.extent), sizeof(Extent));

		uint32_t dataSize = obj.extent.width * obj.extent.height * 4;
		
		uint8_t* data = new uint8_t[dataSize];
		inFile.read(reinterpret_cast<char*>(data), dataSize);

		obj.pixeldata = new uint8_t[dataSize];
		memcpy(obj.pixeldata, data, dataSize);

		delete[] data;
	}
	
	bool GetUUIDFromAssetFile(const AssetFilePath& path, UUID& Id, std::stringstream* sstr)
	{
		std::ifstream fin(path);

		std::stringstream stream;
		stream << fin.rdbuf();
		fin.close();

		YAML::Node data = YAML::Load(stream.str());

		if (!data["Asset ID"])
			return false;
		
		Id = UUID(data["Asset ID"].as<uint64_t>());
		
		if (sstr)
			*sstr << stream.str();

		return true;
	}
}

namespace Nexus::Importer
{
	#define FIND_VALUE_BY_NAME(data,name) data.values.find(name) != data.values.end()

	void CalculateTangentAndBiTangent(Nexus::MeshVertex& v0, Nexus::MeshVertex& v1, Nexus::MeshVertex& v2, bool foundTangent, bool foundBiTangent)
	{
		glm::vec3 deltaPos1 = v1.position - v0.position;
		glm::vec3 deltaPos2 = v2.position - v0.position;

		glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

		glm::vec3 tangent{}, bitangent{};

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		if (!foundTangent)
		{
			tangent.x = f * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
			tangent.y = f * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
			tangent.z = f * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);
			tangent = glm::normalize(tangent);

			v0.tangent = tangent; v1.tangent = tangent; v2.tangent = tangent;
		}

		if (!foundBiTangent)
		{
			bitangent.x = f * (-deltaUV2.x * deltaPos1.x + deltaUV1.x * deltaPos2.x);
			bitangent.y = f * (-deltaUV2.x * deltaPos1.y + deltaUV1.x * deltaPos2.y);
			bitangent.z = f * (-deltaUV2.x * deltaPos1.z + deltaUV1.x * deltaPos2.z);
			bitangent = glm::normalize(bitangent);

			v0.bitangent = bitangent; v1.bitangent = bitangent; v2.bitangent = bitangent;
		}
	}
	
	bool LoadglTF(const AssetFilePath& filepath, MeshSpecification* specs)
	{
		tinygltf::Model scene;
		tinygltf::TinyGLTF Importer;

		// Importing glTF
		{
			std::string error, warning;

			bool glbFile = false;

			if (filepath.extension().string() == ".glb")
				glbFile = true;
			else if (filepath.extension().string() != ".gltf")
			{
				specs = nullptr;
				NEXUS_LOG("glTF Importer Error", "Incorrect File Extentsion");
				return false;
			}

			bool success = glbFile ? Importer.LoadBinaryFromFile(&scene, &error, &warning, filepath.string()) : Importer.LoadASCIIFromFile(&scene, &error, &warning, filepath.string());

			if (!warning.empty())
			{
				NEXUS_LOG("glTF Importer Error", "AssetImporter::glTF Warning - %s", warning.c_str());
			}

			if (!error.empty())
			{
				NEXUS_LOG("glTF Importer Error", "AssetImporter::glTF Error - %s", error.c_str());
			}

			if (!success)
			{
				NEXUS_LOG("glTF Importer Error", "AssetImporter::glTF Failed to Load File - %s", filepath.string().c_str());
				return false;
			}
		}

		// Mesh
		{
			size_t totalVertices = 0;
			
			for (auto& mesh : scene.meshes)
			{
				for (auto& primitive : mesh.primitives)
				{
					bool foundTangent = false;
					bool foundBiTangent = false;

					auto& submesh = specs->elements.emplace_back();

					// Vertices
					const float* positionBuffer = nullptr;
					const float* normalBuffer = nullptr;
					const float* texCoordBuffer = nullptr;
					const float* tangentBuffer = nullptr;
					const float* bitangentBuffer = nullptr;

					size_t vertexCount = 0;

					auto p = primitive.attributes.find("POSITION");
					if (p != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[p->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						positionBuffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);

						vertexCount = accessor.count;
					}

					auto n = primitive.attributes.find("NORMAL");
					if (n != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[n->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						normalBuffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
					}

					auto t = primitive.attributes.find("TEXCOORD_0");
					if (t != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[t->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						texCoordBuffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
					}

					auto a = primitive.attributes.find("TANGENT");
					if (a != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[a->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						tangentBuffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
						foundTangent = true;
					}

					auto b = primitive.attributes.find("BITANGENT");
					if (b != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[b->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						bitangentBuffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
						foundBiTangent = true;
					}

					submesh.Vertices.resize(vertexCount);
					for (size_t i = 0; i < vertexCount; i++)
					{
						auto& vertex = submesh.Vertices[i];

						vertex.position = glm::make_vec3(&positionBuffer[i * 3]);

						if (normalBuffer)
							vertex.normal = glm::make_vec3(&normalBuffer[i * 3]);
						else
							vertex.normal = glm::vec3(0.f);

						if (texCoordBuffer)
							vertex.texCoord = glm::make_vec2(&texCoordBuffer[i * 2]);
						else
							vertex.texCoord = glm::vec2(0.f);

						if (tangentBuffer)
							vertex.tangent = glm::make_vec3(&tangentBuffer[i * 3]);

						if (bitangentBuffer)
							vertex.bitangent = glm::make_vec3(&bitangentBuffer[i * 3]);
					}

					// Indices
					{
						auto& accessor = scene.accessors[primitive.indices];
						auto& view = scene.bufferViews[accessor.bufferView];
						auto& buffer = scene.buffers[view.buffer];

						uint32_t* indexbuffer = nullptr;
						uint32_t indexCount = (uint32_t)accessor.count;

						submesh.Indices.resize(indexCount);

						if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						{
							const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
							for (size_t index = 0; index < accessor.count; index += 3)
							{
								int a = buf[index];
								int b = buf[index + 1];
								int c = buf[index + 2];

								submesh.Indices[index] = (uint32_t)totalVertices + a;
								submesh.Indices[index + 1] = (uint32_t)totalVertices + b;
								submesh.Indices[index + 2] = (uint32_t)totalVertices + c;

								if (!foundTangent || !foundBiTangent)
								{
									CalculateTangentAndBiTangent(submesh.Vertices[a], submesh.Vertices[b], submesh.Vertices[c], foundTangent, foundBiTangent);
								}
							}
						}
						else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
						{
							const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
							for (size_t index = 0; index < accessor.count; index += 3)
							{
								int a = buf[index];
								int b = buf[index + 1];
								int c = buf[index + 2];

								submesh.Indices[index] = (uint16_t)totalVertices + a;
								submesh.Indices[index + 1] = (uint16_t)totalVertices + b;
								submesh.Indices[index + 2] = (uint16_t)totalVertices + c;

								if (!foundTangent || !foundBiTangent)
								{
									CalculateTangentAndBiTangent(submesh.Vertices[a], submesh.Vertices[b], submesh.Vertices[c], foundTangent, foundBiTangent);
								}
							}
						}
						else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
						{
							const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
							for (size_t index = 0; index < accessor.count; index += 3)
							{
								int a = buf[index];
								int b = buf[index + 1];
								int c = buf[index + 2];

								submesh.Indices[index] = (uint8_t)totalVertices + a;
								submesh.Indices[index + 1] = (uint8_t)totalVertices + b;
								submesh.Indices[index + 2] = (uint8_t)totalVertices + c;

								if (!foundTangent || !foundBiTangent)
								{
									CalculateTangentAndBiTangent(submesh.Vertices[a], submesh.Vertices[b], submesh.Vertices[c], foundTangent, foundBiTangent);
								}
							}
						}
						else
						{
							auto err = "GLTF Indices Type Not Supported" + accessor.componentType;
							NEXUS_ASSERT(true, err);
						}
					}

					totalVertices += vertexCount;
				}
			}
		}

		NEXUS_LOG("glTF Importer", "Successfully Loaded File - %s", filepath.string().c_str());
		return true;
	}

	bool LoadTexture(const AssetFilePath& filepath, TextureSpecification* specs)
	{
		int w, h, c;
		uint8_t* pixels = stbi_load(filepath.string().c_str(), &w, &h, &c, 4);

		if (!pixels)
			return false;

		specs->extent.width = (uint32_t)w;
		specs->extent.height = (uint32_t)h;
		specs->pixeldata = new uint8_t[w * h * 4];

		memcpy(specs->pixeldata, pixels, (size_t)w * h * 4);

		stbi_image_free(pixels);

		return true;
	}
}

bool Nexus::MeshAsset::Import(const AssetFilePath& sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath)
{
	MeshSpecification data;
	if (!Importer::LoadglTF(sourcefilepath, &data))
		return false;

	UUID Id;
	
	auto assetFile = AssetPath.string() + "\\" + sourcefilepath.filename().stem().string() + AssetExtension;
	auto binFile = BinPath.string() + "\\" + sourcefilepath.filename().stem().string() + BinExtension;

	// Meta
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Asset ID" << YAML::Value << Id.operator size_t	();
			out << YAML::Key << "Asset Type" << YAML::Value << Utils::GetStringFromType(m_Type);
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
		
		Utils::SerializeMeshAsset(fout, data);
		fout.close();
	}

	return true;
}

bool Nexus::MeshAsset::Load(const AssetFilePath& AssetPath)
{
	std::stringstream stream;
	UUID Id(true);

	if (!Utils::GetUUIDFromAssetFile(AssetPath, Id, &stream))
		return false;

	YAML::Node data = YAML::Load(stream.str());
	
	AssetType Type = Utils::GetTypeFromString(data["Asset Type"].as<std::string>());
	if (Type != AssetType::Mesh)
		return false;

	auto ref = data["Asset Ref"];
	auto binpath = ref["Asset Bin"].as<std::string>();

	std::ifstream inFile(binpath, std::ios::binary);
	
	if (!inFile.is_open())
		return false;

	while (inFile.peek() != EOF) 
		Utils::DeserializeMeshAsset(inFile, m_Specs);
	
	inFile.close();
	
	m_Name = AssetPath.filename().stem().generic_string();
	return true;
}

bool Nexus::TextureAsset::Import(const AssetFilePath& Sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath)
{
	TextureSpecification data{};
	if (!Importer::LoadTexture(Sourcefilepath, &data))
		return false;

	UUID Id;
	auto assetFile = AssetPath.string() + "\\" + Sourcefilepath.filename().stem().string() + AssetExtension;
	auto binFile = BinPath.string() + "\\" + Sourcefilepath.filename().stem().string() + BinExtension;

	// Meta
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Asset ID" << YAML::Value << Id.operator size_t();
			out << YAML::Key << "Asset Type" << YAML::Value << Utils::GetStringFromType(m_Type);
			{
				out << YAML::Key << "Asset Ref";
				out << YAML::BeginMap;
				out << YAML::Key << "Asset Source" << YAML::Value << Sourcefilepath.string();
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

		Utils::SerializeTextureAsset(fout, data);
		fout.close();
	}

	return true;
}

bool Nexus::TextureAsset::Load(const AssetFilePath& AssetPath)
{
	std::stringstream stream;
	UUID Id(false);

	if (!Utils::GetUUIDFromAssetFile(AssetPath, Id, &stream))
		return false;

	YAML::Node data = YAML::Load(stream.str());

	AssetType Type = Utils::GetTypeFromString(data["Asset Type"].as<std::string>());
	if (Type != AssetType::Texture)
		return false;

	auto ref = data["Asset Ref"];
	auto binpath = ref["Asset Bin"].as<std::string>();

	std::ifstream inFile(binpath, std::ios::binary);

	if (!inFile.is_open())
		return false;

	while (inFile.peek() != EOF)
		Utils::DeserializeTextureAsset(inFile, m_Specs);

	inFile.close();

	m_Name = AssetPath.filename().stem().generic_string();
	return true;
}