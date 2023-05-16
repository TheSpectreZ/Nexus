#include "nxpch.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Texture.h"
#include "Scene/Material.h"

#include "Assets/Importer/glTFImporter.h"
#include "Assets/AssetManager.h"

Nexus::Ref<Nexus::StaticMesh> Nexus::StaticMesh::Create(const std::string& filepath, std::vector<UUID>* Materials)
{
	Importer::glTF::glTFSceneData data{};
	std::filesystem::path path = filepath;

	NEXUS_ASSERT((!Importer::glTF::Load(path, &data)), "Failed To Load glTF Static Mesh");
	
	Ref<StaticMesh> mesh = CreateRef<StaticMesh>();

	std::unordered_map<uint32_t, UUID> ImageIDs;
	for (uint32_t i = 0; i < (uint32_t)data.images.size(); i++)
	{
		std::filesystem::path imagePath = path.parent_path() / data.images[i].sourcePath;
		auto [Tex, Id] = AssetManager::Load<Texture>(imagePath.string());
		ImageIDs[i] = Id;
	}

	std::unordered_map<uint32_t, UUID> SamplerIDs;
	for (uint32_t i = 0; i < (uint32_t)data.samplers.size(); i++)
	{
		auto& sampler = data.samplers[i];

		auto [Samp, Id] = AssetManager::Load<Sampler>(sampler.Near, sampler.Far, sampler.U, sampler.V, sampler.W);
		SamplerIDs[i] = Id;
	}

	std::unordered_map<uint32_t, UUID> MaterialIDs;
	for (uint32_t i = 0; i < (uint32_t)data.materials.size(); i++)
	{
		auto& material = data.materials[i];

		MaterialCreateInfo Info{};
		Info.albedoColor = material.albedoColor;
		Info.metalness = material.metallic;
		Info.roughness = material.roughness;

		if(material.albedoTexture != UINT32_MAX)
		{
			Info.albedo.Image = ImageIDs[data.textures[material.albedoTexture].Image];
			Info.albedo.Sampler = SamplerIDs[data.textures[material.albedoTexture].Sampler];
			Info.albedo.TexCoord = material.textureCoords.albedo;
			Info.useAlb = 1.f;
		}
		else
		{
			Info.useAlb = 0.f;
			Info.albedo = { 0,1 };
		}
		
		if(material.metallicRoughnessTexture != UINT32_MAX)
		{
			Info.metallicRoughness.Image = ImageIDs[data.textures[material.metallicRoughnessTexture].Image];
			Info.metallicRoughness.Sampler = SamplerIDs[data.textures[material.metallicRoughnessTexture].Sampler];
			Info.metallicRoughness.TexCoord = material.textureCoords.metallicRoughness;
			Info.useMR = 1.f;
		}
		else
		{
			Info.useMR = 0.f;
			Info.metallicRoughness = { 0,1 };
		}

		auto [Mat, Id] = AssetManager::Load<Material>(Info);
		MaterialIDs[i] = Id;

		Materials->push_back(Id);
	}

	for (auto& meshes : data.meshes)
	{
		for (auto& sb : meshes.submeshes)
		{
			auto& submesh = mesh->m_SubMeshes.emplace_back();

			submesh.material = MaterialIDs[sb.materialIndex];
			submesh.vb = StaticBuffer::Create((uint32_t)sb.vertices.size() * sizeof(StaticMeshVertex), BufferType::Vertex, sb.vertices.data());
			submesh.ib = StaticBuffer::Create((uint32_t)sb.indices.size() * sizeof(uint32_t), BufferType::Index, sb.indices.data());
		}
	}

	return mesh;
}
