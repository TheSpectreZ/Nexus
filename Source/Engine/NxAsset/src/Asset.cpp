#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "NxCore/UUID.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxAsset/Asset.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "nlohmann/json.hpp"

static std::string MeshExtension = ".NxMesh";
static std::string TextureExtension = ".NxTex";
static std::string MaterialExtension = ".NxMat";
static std::string AnimExtension = ".NxAnim";

static std::string BinExtension = ".NxBin";

namespace Nexus::Utils
{
	bool ImportMesh(const AssetFilePath& dstFolder, const std::vector<Meshing::Mesh>& meshes,const std::string& name)
	{
		nlohmann::json Json;
		Json["UUID"] = (uint64_t)UUID();
		Json["AssetType"] = "Mesh";
		
		nlohmann::json Mesh;
		Mesh["MeshCount"] = meshes.size();

		Json["Meta"] = Mesh;
		
		std::string JsonDump = Json.dump(4);

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + name + MeshExtension);
		
		std::ofstream stream(file, std::ios::binary);
		if (!stream.is_open())
			return false;

		uint32_t size = (uint32_t)JsonDump.size();
		stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		stream.write(JsonDump.c_str(), JsonDump.length());
		
		for(auto& mesh : meshes)
		{
			size = (uint32_t)mesh.vertices.size();
			stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(mesh.vertices.data()), size * sizeof(Meshing::Vertex));
			
			size = (uint32_t)mesh.indices.size();
			stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(mesh.indices.data()), size * sizeof(uint32_t));
			
			size = (uint32_t)mesh.submeshes.size();
			stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(mesh.submeshes.data()), size * sizeof(Meshing::Submesh));
		}

		stream.close();
		return true;
	}

	bool ImportImage(const AssetFilePath& dstFolder, const Meshing::Image* image)
	{
		nlohmann::json Json;
		Json["UUID"] = (uint64_t)UUID();
		Json["AssetType"] = "Image";

		nlohmann::json Image;
		Image["ImageWidth"] = image->width;
		Image["ImageHeight"] = image->height;
		Image["ImageChannels"] = image->channels;

		Json["Meta"] = Image;

		std::string JsonDump = Json.dump(4);

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + image->fileName + TextureExtension);

		std::ofstream stream(file, std::ios::binary);
		if (!stream.is_open())
			return false;

		uint32_t size = (uint32_t)JsonDump.size();
		stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		stream.write(JsonDump.c_str(), JsonDump.length());

		size = image->width * image->height * image->channels;
		stream.write(reinterpret_cast<const char*>(image->pixels.data()), size * sizeof(uint8_t));

		stream.close();

		return true;
	}
}

bool Nexus::Importer::ImportGLTF(const AssetFilePath& path, const AssetFilePath& dstFolder,const std::string& Name)
{
	Meshing::Scene scene;
	Meshing::LoadSceneFromFile(path, &scene);

	if (!std::filesystem::exists(dstFolder))
		std::filesystem::create_directories(dstFolder);

	Utils::ImportMesh(dstFolder, scene.meshes, Name);

	for (auto& Image : scene.images)
		Utils::ImportImage(dstFolder, &Image);

	return true;
}

bool NEXUS_ASSET_API Nexus::Importer::ImportImage(const AssetFilePath& path, const AssetFilePath& dstFolder, const std::string& Name)
{
	if (path.extension().string() != ".png" && path.extension().string() != ".jpg")
		return false;
	
	Meshing::Image i;

	int w, h, c;
	auto pixels = stbi_load(path.generic_string().c_str(), &w, &h, &c, 4);

	i.fileName = Name;
	i.width = w;
	i.height = h;
	i.channels = 4;

	i.pixels.resize((size_t)w * h * 4);
	memcpy(i.pixels.data(), pixels, i.pixels.size() * sizeof(uint8_t));
	stbi_image_free(pixels);

	if (!std::filesystem::exists(dstFolder))
		std::filesystem::create_directories(dstFolder);

	return Utils::ImportImage(dstFolder, &i);
}

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadMesh(const AssetFilePath& path, std::vector<Meshing::Mesh>& meshes)
{
	UUID Id(true);

	if (path.extension().string() != MeshExtension)
		return { false,Id };

	std::ifstream stream(path,std::ios::binary);

	if (!stream.is_open())
		return { false,Id };

	{
		uint32_t jsonlen = 0;
		stream.read(reinterpret_cast<char*>(&jsonlen), sizeof(uint32_t));
		
		std::string JsonDump;
		JsonDump.resize(jsonlen);
		stream.read(&JsonDump[0], jsonlen);

		nlohmann::json Json = nlohmann::json::parse(JsonDump);
		
		if (!Json.contains("AssetType"))
			return { false,Id };

		auto type = Json["AssetType"].get<std::string>();
		if (type != "Mesh")
			return { false,Id };

		auto uid = Json["UUID"].get<uint64_t>();
		Id = UUID(uid);

		auto& meta = Json["Meta"];
		auto meshCount = meta["MeshCount"].get<uint32_t>();

		meshes.resize(meshCount);
		for (auto& mesh : meshes)
		{
			uint32_t numVertices = 0;
			stream.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));

			mesh.vertices.resize(numVertices);
			stream.read(reinterpret_cast<char*>(mesh.vertices.data()), numVertices * sizeof(Meshing::Vertex));

			uint32_t numIndices = 0;
			stream.read(reinterpret_cast<char*>(&numIndices), sizeof(uint32_t));

			mesh.indices.resize(numIndices);
			stream.read(reinterpret_cast<char*>(mesh.indices.data()), numIndices * sizeof(uint32_t));

			uint32_t numSubmeshes = 0;
			stream.read(reinterpret_cast<char*>(&numSubmeshes), sizeof(uint32_t));

			mesh.submeshes.resize(numSubmeshes);
			stream.read(reinterpret_cast<char*>(mesh.submeshes.data()), numSubmeshes * sizeof(Meshing::Submesh));
		}
	}
	stream.close();

	return { true,Id };
}

std::pair<bool, Nexus::UUID>NEXUS_ASSET_API Nexus::Importer::LoadImage(const AssetFilePath& path,Meshing::Image& image)
{
	UUID Id(true);

	if (path.extension().string() != TextureExtension)
		return { false,Id };

	std::ifstream stream(path, std::ios::binary);

	if (!stream.is_open())
		return { false,Id };

	{
		uint32_t jsonlen = 0;
		stream.read(reinterpret_cast<char*>(&jsonlen), sizeof(uint32_t));

		std::string JsonDump;
		JsonDump.resize(jsonlen);
		stream.read(&JsonDump[0], jsonlen);

		nlohmann::json Json = nlohmann::json::parse(JsonDump);

		if (!Json.contains("AssetType"))
			return { false,Id };

		auto type = Json["AssetType"].get<std::string>();
		if (type != "Image")
			return { false,Id };

		auto uid = Json["UUID"].get<uint64_t>();
		Id = UUID(uid);

		auto& meta = Json["Meta"];

		image.width = meta["ImageWidth"].get<uint32_t>();
		image.height = meta["ImageHeight"].get<uint32_t>();
		image.channels = meta["ImageChannels"].get<uint32_t>();

		uint32_t size = image.width * image.height * image.channels;

		image.pixels.resize(size);
		stream.read(reinterpret_cast<char*>(image.pixels.data()), size * sizeof(uint8_t));
	}

	return { true,Id };
}
