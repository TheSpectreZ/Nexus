#include "nxpch.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Texture.h"
#include "Scene/Material.h"

#include "Assets/Importer/glTFImporter.h"
#include "Assets/AssetManager.h"

Nexus::Ref<Nexus::StaticMesh> Nexus::StaticMesh::Create(const std::string& filepath)
{
	Importer::glTF::glTFSceneData data{};
	std::filesystem::path path = filepath;

	NEXUS_ASSERT((!Importer::glTF::Load(path, &data)), "Failed To Load glTF Static Mesh");
	
	Ref<StaticMesh> mesh = CreateRef<StaticMesh>();

	for (auto& meshes : data.meshes)
	{
		for (auto& sb : meshes.submeshes)
		{
			auto& submesh = mesh->m_SubMeshes.emplace_back();
			
			submesh.material = sb.materialIndex;
			submesh.vb = StaticBuffer::Create((uint32_t)sb.vertices.size() * sizeof(StaticMeshVertex), BufferType::Vertex, sb.vertices.data());
			submesh.ib = StaticBuffer::Create((uint32_t)sb.indices.size() * sizeof(uint32_t), BufferType::Index, sb.indices.data());
		}
	}

	std::unordered_map<uint32_t, UUID> ImageIDs;
	for (uint32_t i = 0; i < (uint32_t)data.images.size(); i++)
	{
		auto [Tex, Id] = AssetManager::Load<Texture>(data.images[i].sourcePath);
		ImageIDs[i] = Id;
	}

	std::unordered_map<uint32_t, UUID> SamplerIDs;
	for (uint32_t i = 0; i < (uint32_t)data.samplers.size(); i++)
	{
		auto& sampler = data.samplers[i];

		auto [Samp, Id] = AssetManager::Load<Sampler>(sampler.Near, sampler.Far, sampler.U, sampler.V, sampler.W);
		SamplerIDs[Id];
	}

	// Make Materials

	return mesh;
}
