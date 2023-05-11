#include "nxpch.h"
#include "Mesh.h"

#include "Vertex.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Renderer.h"

Nexus::Ref<Nexus::StaticMesh> Nexus::StaticMesh::LoadWithAssimp(const char* Filepath)
{
	static Assimp::Importer importer;

	uint32_t flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes;
	const aiScene* scene = importer.ReadFile(Filepath, flags);

	if (!scene)
	{
		std::string err = "Failed To Load File with Assimp: " + std::string(Filepath) + " L " + importer.GetErrorString();
		NEXUS_ASSERT(1, err.c_str());
	}

	std::vector<StaticMeshVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			auto& vertex = m_Vertices.emplace_back();

			vertex.position.x = mesh->mVertices[j].x;
			vertex.position.y = mesh->mVertices[j].y;
			vertex.position.z = mesh->mVertices[j].z;

			vertex.normal.x = mesh->mNormals[j].x;
			vertex.normal.y = mesh->mNormals[j].y;
			vertex.normal.z = mesh->mNormals[j].z;
		}

		for (uint32_t k = 0; k < mesh->mNumFaces; k++)
		{
			aiFace face = mesh->mFaces[k];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				m_Indices.push_back(face.mIndices[j]);
			}
		}

	}

	if (m_Vertices.empty() || m_Indices.empty())
	{
		std::string s = Filepath;
		s += " :: Is Empty";

		NEXUS_ASSERT(1, s.c_str());
	}

	NEXUS_LOG_TRACE("Mesh Loaded: {0} | vertices-{1} | indices-{2}", Filepath, m_Vertices.size(), m_Indices.size());

	Ref<StaticMesh> sMesh = CreateRef<StaticMesh>();

	sMesh->m_Vb = StaticBuffer::Create((uint32_t)m_Vertices.size() * sizeof(StaticMeshVertex), BufferType::Vertex, m_Vertices.data());
	sMesh->m_Ib = StaticBuffer::Create((uint32_t)m_Indices.size() * sizeof(uint32_t), BufferType::Index, m_Indices.data());

	Renderer::TransferMeshToGPU(sMesh);

	return sMesh;
}
