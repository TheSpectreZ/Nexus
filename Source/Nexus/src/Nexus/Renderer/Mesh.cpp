#include "nxpch.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Renderer/Command.h"

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

	Ref<StaticMesh> sMesh = CreateRef<StaticMesh>();

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			auto& vertex = sMesh->m_Vertices.emplace_back();

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
				sMesh->m_Indices.push_back(face.mIndices[j]);
			}
		}

	}

	if (sMesh->m_Vertices.empty() || sMesh->m_Indices.empty())
	{
		std::string s = Filepath;
		s += " :: Is Empty";

		NEXUS_ASSERT(1, s.c_str());
	}

	NEXUS_LOG_TRACE("Mesh Loaded: {0} | vertices-{1} | indices-{2}", Filepath, sMesh->m_Vertices.size(), sMesh->m_Indices.size());

	sMesh->m_Vb = StaticBuffer::Create(sMesh->m_Vertices.size() * sizeof(StaticMeshVertex), BufferType::VERTEX, sMesh->m_Vertices.data());
	sMesh->m_Ib = StaticBuffer::Create(sMesh->m_Indices.size() * sizeof(uint32_t), BufferType::INDEX, sMesh->m_Indices.data());

	Command::TransferStaticMesh(sMesh);

	return sMesh;
}

Nexus::Ref<Nexus::StaticMesh> Nexus::StaticMesh::LoadPlane()
{
	Ref<StaticMesh> sMesh = CreateRef<StaticMesh>();

	// Mesh data
	{
		sMesh->m_Vertices.resize(4);
		sMesh->m_Vertices[0].position = glm::vec3(0.f, 0.f, 0.f); sMesh->m_Vertices[0].normal = glm::vec3(1.f);
		sMesh->m_Vertices[1].position = glm::vec3(1.f, 0.f, 0.f); sMesh->m_Vertices[1].normal = glm::vec3(1.f);
		sMesh->m_Vertices[2].position = glm::vec3(1.f, 1.f, 0.f); sMesh->m_Vertices[2].normal = glm::vec3(1.f);
		sMesh->m_Vertices[3].position = glm::vec3(0.f, 1.f, 0.f); sMesh->m_Vertices[3].normal = glm::vec3(1.f);

		sMesh->m_Indices.resize(6);
		sMesh->m_Indices[0] = 0;
		sMesh->m_Indices[1] = 1;
		sMesh->m_Indices[2] = 2;
		sMesh->m_Indices[3] = 2;
		sMesh->m_Indices[4] = 3;
		sMesh->m_Indices[5] = 0;
	}

	sMesh->m_Vb = StaticBuffer::Create(sMesh->m_Vertices.size() * sizeof(StaticMeshVertex), BufferType::VERTEX, sMesh->m_Vertices.data());
	sMesh->m_Ib = StaticBuffer::Create(sMesh->m_Indices.size() * sizeof(uint32_t), BufferType::INDEX, sMesh->m_Indices.data());

	Command::TransferStaticMesh(sMesh);

	return sMesh;
}

void Nexus::StaticMesh::Destroy()
{
	m_Vb->~StaticBuffer();
	m_Ib->~StaticBuffer();
}