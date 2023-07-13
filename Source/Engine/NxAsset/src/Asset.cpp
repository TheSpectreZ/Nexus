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
	bool ImportMesh(const AssetFilePath& dstFolder,Meshing::Mesh& mesh,const std::string& name)
	{
		nlohmann::json Json;
		Json["UUID"] = (uint64_t)UUID();
		Json["AssetType"] = "Mesh";
		
		nlohmann::json Mesh;
		Mesh["VertexCount"] = mesh.vertices.size();
		Mesh["IndexCount"] = mesh.indices.size();
		Mesh["SubmeshCount"] = mesh.submeshes.size();

		Json["Meta"] = Mesh;
		
		std::string JsonDump = Json.dump(4);

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + name + MeshExtension);
		
		std::ofstream stream(file, std::ios::binary);
		if (!stream.is_open())
			return false;

		uint32_t size = (uint32_t)JsonDump.size();
		stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		stream.write(JsonDump.c_str(), JsonDump.length());
		
		stream.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.size() * sizeof(Meshing::Vertex));	
		stream.write(reinterpret_cast<const char*>(mesh.indices.data()), mesh.indices.size() * sizeof(uint32_t));
		stream.write(reinterpret_cast<const char*>(mesh.submeshes.data()), mesh.submeshes.size() * sizeof(Meshing::Submesh));
		
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

	bool ImportTexture(const AssetFilePath& dstFolder, const Meshing::Texture* texture)
	{
		nlohmann::json Json;
		Json["UUID"] = (uint64_t)UUID();
		Json["AssetType"] = "Texture";

		nlohmann::json Tex;
		Tex["Width"] = texture->image.width;
		Tex["Height"] = texture->image.height;
		Tex["Channels"] = texture->image.channels;
		Tex["Sampler"] = texture->samplerHash;

		Json["Meta"] = Tex;

		std::string JsonDump = Json.dump(4);

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + texture->image.fileName + TextureExtension);

		std::ofstream stream(file, std::ios::binary);
		if (!stream.is_open())
			return false;

		uint32_t size = (uint32_t)JsonDump.size();
		stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		stream.write(JsonDump.c_str(), JsonDump.length());

		stream.write(reinterpret_cast<const char*>(texture->image.pixels.data()), texture->image.pixels.size() * sizeof(uint8_t));

		stream.close();

		return true;
	}

	Nexus::UUID ImportMaterial(const AssetFilePath& dstFolder, const Meshing::Material* material, const std::vector<Meshing::Texture>& textures)
	{
		UUID uid;

		nlohmann::json Json;
		Json["UUID"] = (uint64_t)uid;
		Json["AssetType"] = "Material";

		nlohmann::json Material;

		auto TexFolder = dstFolder.parent_path() / "Textures";
		
		AssetFilePath path = TexFolder;
		if (material->specularGlossiness.albedoTexture != UINT64_MAX || material->metalicRoughness.albedoTexture != UINT64_MAX)
		{
			if (material->specularGlossiness.support)
				path /= AssetFilePath(textures[material->specularGlossiness.albedoTexture].image.fileName + TextureExtension);
			else
				path /= AssetFilePath(textures[material->metalicRoughness.albedoTexture].image.fileName + TextureExtension);
			Material["AlbedoMap"] = path.generic_string();
		}

		if (material->normalTexture != UINT64_MAX)
		{
			path = TexFolder / textures[material->normalTexture].image.fileName;
			path += TextureExtension;
			Material["NormalMap"] = path.generic_string();
		}

		if (material->occulsionTexture != UINT64_MAX)
		{
			path = TexFolder / textures[material->occulsionTexture].image.fileName;
			path += TextureExtension;
			Material["OcculsionMap"] = path.generic_string();
		}

		if(material->emissiveTexture != UINT64_MAX)
		{
			path = TexFolder / textures[material->emissiveTexture].image.fileName;
			path += TextureExtension; 
			Material["EmissiveMap"] = path.generic_string();
		}

		if(material->metalicRoughness.metallicRoughnessTexture != UINT64_MAX)
		{
			path = TexFolder / textures[material->metalicRoughness.metallicRoughnessTexture].image.fileName;
			path += TextureExtension;
			Material["MetallicRoughnessMap"] = path.generic_string();
		}

		if(material->specularGlossiness.specularGlossinessTexture != UINT64_MAX)
		{
			path = TexFolder / textures[material->specularGlossiness.specularGlossinessTexture].image.fileName;
			path += TextureExtension;
			Material["SpecularGlossinessMap"] = path.generic_string();
		}

		glm::vec4 albedoColor;
		if (material->specularGlossiness.support)
			albedoColor = material->specularGlossiness.aldeboColor;
		else
			albedoColor = material->metalicRoughness.albedoColor;

		Material["SpecularSupport"] = material->specularGlossiness.support;
		Material["AlbedoColor"]["r"] = albedoColor.x;
		Material["AlbedoColor"]["g"] = albedoColor.y;
		Material["AlbedoColor"]["b"] = albedoColor.z;
		Material["AlbedoColor"]["a"] = albedoColor.w;

		Material["Roughness"] = material->metalicRoughness.roughness;
		Material["Metalness"] = material->metalicRoughness.metallic;
		Material["Glossiness"] = material->specularGlossiness.glossiness;

		Material["EmissiveColor"]["r"] = material->emissiveColor.x;
		Material["EmissiveColor"]["g"] = material->emissiveColor.y;
		Material["EmissiveColor"]["b"] = material->emissiveColor.z;
		
		Material["Specular"]["r"] = material->specularGlossiness.specular.x;
		Material["Specular"]["g"] = material->specularGlossiness.specular.y;
		Material["Specular"]["b"] = material->specularGlossiness.specular.z;

		Json["Meta"] = Material;

		std::string JsonDump = Json.dump(4);

		std::string Name = material->Name;
		if (Name.empty())
			Name = "UnNamed-Material";

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + Name + MaterialExtension);

		if (std::filesystem::exists(file))
			file.replace_filename(std::string(Name + "- Copy" + MaterialExtension));

		std::ofstream stream(file);
		stream << JsonDump;
		
		return uid;
	}
}

bool Nexus::Importer::ImportGLTF(const AssetFilePath& path, const AssetFilePath& dstFolder,const std::string& Name)
{
	Meshing::Scene scene;
	if (!Meshing::LoadSceneFromFile(path, &scene))
		return false;

	if (!std::filesystem::exists(dstFolder))
		std::filesystem::create_directories(dstFolder);

	auto TexFolder = dstFolder / "Textures";
	if (!std::filesystem::exists(TexFolder))
		std::filesystem::create_directories(TexFolder);

	for (auto& Tex : scene.textures)
		Utils::ImportTexture(TexFolder, &Tex);

	auto MatFolder = dstFolder / "Materials";
	if (!std::filesystem::exists(MatFolder))
		std::filesystem::create_directories(MatFolder);

	for (auto& Mat : scene.materials)
		Utils::ImportMaterial(MatFolder, &Mat, scene.textures);

	Utils::ImportMesh(dstFolder, scene.mesh, Name);

	return true;
}

bool Nexus::Importer::ImportImage(const AssetFilePath& path, const AssetFilePath& dstFolder, const std::string& Name)
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

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadMesh(const AssetFilePath& path, Meshing::Mesh& mesh)
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
		auto vc = meta["VertexCount"].get<uint32_t>();
		auto ic = meta["IndexCount"].get<uint32_t>();
		auto sc = meta["SubmeshCount"].get<uint32_t>();

		mesh.vertices.resize(vc);
		mesh.indices.resize(ic);
		mesh.submeshes.resize(sc);

		stream.read(reinterpret_cast<char*>(mesh.vertices.data()), vc * sizeof(Meshing::Vertex));
		stream.read(reinterpret_cast<char*>(mesh.indices.data()), ic * sizeof(uint32_t));
		stream.read(reinterpret_cast<char*>(mesh.submeshes.data()), sc * sizeof(Meshing::Submesh));
	}

	stream.close();

	return { true,Id };
}

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadImage(const AssetFilePath& path,Meshing::Image& image)
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
	stream.close();

	return { true,Id };
}

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadTexture(const AssetFilePath& path, Meshing::Texture& texture)
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
		if (type != "Texture")
			return { false,Id };

		auto uid = Json["UUID"].get<uint64_t>();
		Id = UUID(uid);

		auto& meta = Json["Meta"];

		texture.image.width = meta["Width"].get<uint32_t>();
		texture.image.height = meta["Height"].get<uint32_t>();
		texture.image.channels = meta["Channels"].get<uint32_t>();

		if (meta.contains("Sampler"))
			texture.samplerHash = meta["Sampler"].get<uint32_t>();
		else
			texture.samplerHash = 11122; // Look up to Meshing::Sampler 

		uint32_t size = texture.image.width * texture.image.height * texture.image.channels;

		texture.image.pixels.resize(size);
		stream.read(reinterpret_cast<char*>(texture.image.pixels.data()), size * sizeof(uint8_t));
	}
	stream.close();

	return { true,Id };
}

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadMaterial(const AssetFilePath& path, Meshing::Material& material, std::unordered_map<uint8_t, Meshing::Texture>& textures)
{
	UUID UId(true);

	if (path.extension().string() != MaterialExtension)
		return { false,UId };

	std::ifstream fin(path);

	std::stringstream stream;
	stream << fin.rdbuf();
	fin.close();

	nlohmann::json Json = nlohmann::json::parse(stream.str());

	if (!Json.contains("AssetType"))
		return { false,UId };

	auto type = Json["AssetType"].get<std::string>();
	if (type != "Material")
		return { false,UId };

	auto uid = Json["UUID"].get<uint64_t>();
	UId = UUID(uid);

	auto& meta = Json["Meta"];

	material.specularGlossiness.support = meta["SpecularSupport"].get<bool>();
	material.specularGlossiness.glossiness = meta["Glossiness"].get<float>();
	material.metalicRoughness.roughness = meta["Roughness"].get<float>();
	material.metalicRoughness.metallic = meta["Metalness"].get<float>();

	material.emissiveColor.r = meta["EmissiveColor"]["r"].get<float>();
	material.emissiveColor.g = meta["EmissiveColor"]["g"].get<float>();
	material.emissiveColor.b = meta["EmissiveColor"]["b"].get<float>();
	
	material.specularGlossiness.specular.r = meta["Specular"]["r"].get<float>();
	material.specularGlossiness.specular.g = meta["Specular"]["g"].get<float>();
	material.specularGlossiness.specular.b = meta["Specular"]["b"].get<float>();

	glm::vec4 albedo{};
	albedo.r = meta["AlbedoColor"]["r"].get<float>();
	albedo.g = meta["AlbedoColor"]["g"].get<float>();
	albedo.b = meta["AlbedoColor"]["b"].get<float>();

	material.specularGlossiness.aldeboColor = albedo;
	material.metalicRoughness.albedoColor = albedo;

	if (meta.contains("AlbedoMap"))
	{
		auto Path = meta["AlbedoMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[0]);

		if (res)
		{
			if (material.specularGlossiness.support)
				material.specularGlossiness.albedoTexture = UUID(Id);
			else
				material.metalicRoughness.albedoTexture = UUID(Id);
		}
	}

	if (meta.contains("NormalMap"))
	{
		auto Path = meta["NormalMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[3]);

		if (res)
			material.normalTexture = UUID(Id);
	}

	if (meta.contains("OcculsionMap"))
	{
		auto Path = meta["OcculsionMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[4]);

		if (res)
			material.occulsionTexture = UUID(Id);
	}
	
	if (meta.contains("EmissiveMap"))
	{
		auto Path = meta["EmissiveMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[5]);

		if (res)
			material.emissiveTexture = UUID(Id);
	}
	
	if (meta.contains("MetallicRoughnessMap"))
	{
		auto Path = meta["MetallicRoughnessMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[1]);

		if (res)
			material.metalicRoughness.metallicRoughnessTexture = UUID(Id);
	}

	if (meta.contains("SpecularGlossinessMap"))
	{
		auto Path = meta["SpecularGlossinessMap"].get<std::string>();
		auto [res, Id] = LoadTexture(Path, textures[1]);

		if (res)
			material.specularGlossiness.specularGlossinessTexture = UUID(Id);
	}

	return { true,UId };
}
