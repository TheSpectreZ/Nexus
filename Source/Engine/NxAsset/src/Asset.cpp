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
		uint32_t numVertices = obj.mesh.Vertices.size();
		outFile.write(reinterpret_cast<const char*>(&numVertices), sizeof(uint32_t));
		outFile.write(reinterpret_cast<const char*>(obj.mesh.Vertices.data()), numVertices * sizeof(MeshVertex));

		uint32_t numIndices = obj.mesh.Indices.size();
		outFile.write(reinterpret_cast<const char*>(&numIndices), sizeof(uint32_t));
		outFile.write(reinterpret_cast<const char*>(obj.mesh.Indices.data()), numIndices * sizeof(uint32_t));

		uint32_t numSubmeshes = obj.submeshes.size();
		outFile.write(reinterpret_cast<const char*>(&numSubmeshes), sizeof(uint32_t));
		outFile.write(reinterpret_cast<const char*>(obj.submeshes.data()), numSubmeshes * sizeof(SubmeshElement));
	}

	void DeserializeMeshAsset(std::ifstream& inFile, MeshSpecification& obj)
	{
		uint32_t numVertices;
		inFile.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));

		obj.mesh.Vertices.resize(numVertices);
		inFile.read(reinterpret_cast<char*>(obj.mesh.Vertices.data()), numVertices * sizeof(MeshVertex));

		uint32_t numIndices;
		inFile.read(reinterpret_cast<char*>(&numIndices), sizeof(uint32_t));

		obj.mesh.Indices.resize(numIndices);
		inFile.read(reinterpret_cast<char*>(obj.mesh.Indices.data()), numIndices * sizeof(uint32_t));

		uint32_t numSubmeshes;
		inFile.read(reinterpret_cast<char*>(&numSubmeshes), sizeof(uint32_t));

		obj.submeshes.resize(numSubmeshes);
		inFile.read(reinterpret_cast<char*>(obj.submeshes.data()), numSubmeshes * sizeof(SubmeshElement));
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
	
	Nexus::SamplerFilter GetFilterMode(int32_t filter)
	{
		switch (filter)
		{
		case 9728:
			return SamplerFilter::Nearest;
		case 9729:
			return SamplerFilter::Linear;
		case 9984:
			return SamplerFilter::Nearest;
		case 9985:
			return SamplerFilter::Nearest;
		case 9986:
			return SamplerFilter::Linear;
		case 9987:
			return SamplerFilter::Linear;
		default:
			NEXUS_LOG("glTF Importer Error", "Unknown GLTF Filter Mode: %i", filter);
			return SamplerFilter::Linear;
		}
	}

	Nexus::SamplerWrapMode GetWrapMode(int32_t mode)
	{
		switch (mode)
		{
		case 10497:
			return SamplerWrapMode::Repeat;
		case 33071:
			return SamplerWrapMode::Clamped_To_Edge;
		case 33648:
			return SamplerWrapMode::Mirrored_Repeat;
		default:
			NEXUS_LOG("glTF Importer Error", "Unknown GLTF Wrap Mode: {0}", mode);
			return SamplerWrapMode::Repeat;
		}
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
			size_t totalIndex = 0;
			
			std::unordered_map<uint32_t, MeshElement> elements;

			for (auto& mesh : scene.meshes)
			{
				for (auto& primitive : mesh.primitives)
				{
					bool foundTangent = false;
					bool foundBiTangent = false;

					auto& sm = elements[primitive.material];
					
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

					size_t prevSize = sm.Vertices.size();
					sm.Vertices.resize(prevSize + vertexCount);

					for (size_t i = 0; i < vertexCount; i++)
					{
						auto& vertex = sm.Vertices[prevSize + i];

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

					auto& accessor = scene.accessors[primitive.indices];
					auto& view = scene.bufferViews[accessor.bufferView];
					auto& buffer = scene.buffers[view.buffer];

					uint32_t* indexbuffer = nullptr;
					uint32_t indexCount = (uint32_t)accessor.count;

					prevSize = sm.Indices.size();
					sm.Indices.resize(prevSize + indexCount);

					if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index += 3)
						{
							int a = buf[index];
							int b = buf[index + 1];
							int c = buf[index + 2];

							sm.Indices[prevSize + index] = (uint32_t)totalVertices + a;
							sm.Indices[prevSize + index + 1] = (uint32_t)totalVertices + b;
							sm.Indices[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(sm.Vertices[a], sm.Vertices[b], sm.Vertices[c], foundTangent, foundBiTangent);
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

							sm.Indices[prevSize + index] = (uint16_t)totalVertices + a;
							sm.Indices[prevSize + index + 1] = (uint16_t)totalVertices + b;
							sm.Indices[prevSize + index + 2] = (uint16_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(sm.Vertices[a], sm.Vertices[b], sm.Vertices[c], foundTangent, foundBiTangent);
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

							sm.Indices[prevSize + index] = (uint8_t)totalVertices + a;
							sm.Indices[prevSize + index + 1] = (uint8_t)totalVertices + b;
							sm.Indices[prevSize + index + 2] = (uint8_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(sm.Vertices[a], sm.Vertices[b], sm.Vertices[c], foundTangent, foundBiTangent);
							}
						}
					}
					else
					{
						auto err = "GLTF Indices Type Not Supported" + accessor.componentType;
						NEXUS_ASSERT(true, err);
					}

					totalVertices += vertexCount;
					totalIndex += indexCount;
				}
			}

			specs->mesh.Vertices.reserve(totalVertices);
			specs->mesh.Indices.reserve(totalIndex);

			for (auto& [key, value] : elements)
			{
				auto& sm = specs->submeshes.emplace_back();
				sm.materialIndex = key;
				sm.VertexOff = specs->mesh.Vertices.size();
				sm.VertexSize = value.Vertices.size();
				sm.IndexOff = specs->mesh.Indices.size();
				sm.IndexSize = value.Indices.size();

				specs->mesh.Vertices.insert(specs->mesh.Vertices.end(), value.Vertices.begin(), value.Vertices.end());
				specs->mesh.Indices.insert(specs->mesh.Indices.end(), value.Indices.begin(), value.Indices.end());
			}
		}

		// Material
		{
			std::vector<std::string> images;
			
			for (auto& image : scene.images)
			{
				images.emplace_back(image.uri);
			}

			for (auto& sampler : scene.samplers)
			{
				auto& s = specs->materials.samplers.emplace_back();

				s.Near = GetFilterMode(sampler.magFilter);
				s.Far = GetFilterMode(sampler.minFilter);
				s.U = GetWrapMode(sampler.wrapS);
				s.V = GetWrapMode(sampler.wrapT);
				s.W = s.V;
			}

			for (auto& texture : scene.textures)
			{
				auto& t = specs->materials.texElements.emplace_back();

				t.textureId = texture.source;
				t.samplerId = texture.sampler;
			}

			for (auto& material : scene.materials)
			{
				auto& m = specs->materials.matElements.emplace_back();

				if (FIND_VALUE_BY_NAME(material, "baseColorTexture"))
				{
					m.Maps[TextureMapType::Albedo] = material.values["baseColorTexture"].TextureIndex();
					m.albedo = glm::vec4(1.f);
				}

				if (FIND_VALUE_BY_NAME(material, "metallicRoughnessTexture"))
				{
					m.Maps[TextureMapType::MetallicRoughness] = material.values["metallicRoughnessTexture"].TextureIndex();
					
					m.roughness = 1.f;
					m.metalness = 1.f;
				}
				else
				{
					if (FIND_VALUE_BY_NAME(material, "roughnessFactor"))
					{
						m.roughness = (float)material.values["roughnessFactor"].Factor();
					}

					if (FIND_VALUE_BY_NAME(material, "metallicFactor"))
					{
						m.metalness = (float)material.values["metallicFactor"].Factor();
					}
				}

				if (FIND_VALUE_BY_NAME(material, "normalTexture"))
				{
					m.Maps[TextureMapType::Normal] = material.values["normalTexture"].TextureIndex();
				}

				if (FIND_VALUE_BY_NAME(material, "baseColorFactor"))
				{
					m.albedo = glm::make_vec4(material.values["baseColorFactor"].ColorFactor().data());
				}
			}

			specs->materials.textures.resize(images.size());
			for (size_t i = 0; i < images.size(); i++)
			{
				AssetFilePath p = filepath.parent_path().generic_string() + "/" + images[i];
				LoadTexture(p, &specs->materials.textures[i]);
			}
		}

		NEXUS_LOG("glTF Importer", "Successfully Loaded File - %s", filepath.string().c_str());
		return true;
	}
}

bool Nexus::MeshAsset::Import(const AssetFilePath& sourcefilepath, const AssetFilePath& AssetPath, const AssetFilePath& BinPath)
{
	MeshSpecification data;
	if (!Importer::LoadglTF(sourcefilepath, &data))
		return false;

	UUID Id;
	
	auto assetFile = AssetPath.string() + "/" + sourcefilepath.filename().stem().string() + AssetExtension;
	auto binFile = BinPath.string() + "/" + sourcefilepath.filename().stem().string() + BinExtension;

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