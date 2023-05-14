#include "nxpch.h"
#include "Mesh.h"

#include "Vertex.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>

#include "Assets/AssetManager.h"

struct MeshData
{
	struct SubmeshData
	{
		std::vector<Nexus::StaticMeshVertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;
	};

	struct MaterialData
	{
		uint32_t AlbedoTextureIndex;
	};

	std::unordered_map<uint32_t, SubmeshData> submeshData;
	std::unordered_map<uint32_t, MaterialData> materialData;
	std::unordered_map<uint32_t, std::string> imageData;
};

bool LoadGLTF(const std::string& file, MeshData* data)
{
	tinygltf::Model input;
	tinygltf::TinyGLTF context;
	std::string error, warning;

	bool loaded = context.LoadASCIIFromFile(&input, &error, &warning, file);

	if (!warning.empty())
	{
		NEXUS_LOG_ERROR("TinyGLTF Warning: {0}", warning);
	}

	if (!error.empty())
	{
		NEXUS_LOG_ERROR("TinyGLTF Error: {0}", error);
	}

	if (!loaded)
		return false;

	int b = 0;
	for (uint32_t q = 0; q < (uint32_t)input.meshes.size(); q++)
	{
		auto& mesh = input.meshes[q];

		for (uint32_t g = 0; g < mesh.primitives.size(); g++)
		{
			auto& primitive = mesh.primitives[g];

			auto& subdata = data->submeshData[b++];
			subdata.materialIndex = primitive.material;

			// Vertices
			{
				const float* positionBuffer = nullptr;
				const float* normalBuffer = nullptr;
				const float* texCoordBuffer = nullptr;
				size_t vertexCount = 0;

				auto p = primitive.attributes.find("POSITION");
				if (p != primitive.attributes.end())
				{
					auto& accessor = input.accessors[p->second];
					auto& view = input.bufferViews[accessor.bufferView];
					positionBuffer = reinterpret_cast<const float*>(&input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);

					vertexCount = accessor.count;
				}

				auto n = primitive.attributes.find("NORMAL");
				if (n != primitive.attributes.end())
				{
					auto& accessor = input.accessors[n->second];
					auto& view = input.bufferViews[accessor.bufferView];
					normalBuffer = reinterpret_cast<const float*>(&input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
				}

				auto t = primitive.attributes.find("TEXCOORD_0");
				if (t != primitive.attributes.end())
				{
					auto& accessor = input.accessors[t->second];
					auto& view = input.bufferViews[accessor.bufferView];
					texCoordBuffer = reinterpret_cast<const float*>(&input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
				}

				for (size_t i = 0; i < vertexCount; i++)
				{
					Nexus::StaticMeshVertex& vertex = subdata.vertices.emplace_back();

					vertex.position = glm::make_vec3(&positionBuffer[i * 3]);

					if (normalBuffer)
						vertex.normal = glm::make_vec3(&normalBuffer[i * 3]);
					else
						vertex.normal = glm::vec3(0.f);

					if (texCoordBuffer)
						vertex.texCoord = glm::make_vec2(&texCoordBuffer[i * 2]);
					else
						vertex.texCoord = glm::vec2(0.f);
				}
			}

			// Indices
			{
				auto& accessor = input.accessors[primitive.indices];
				auto& view = input.bufferViews[accessor.bufferView];
				auto& buffer = input.buffers[view.buffer];

				uint32_t indexCount = (uint32_t)accessor.count;

				NEXUS_ASSERT((accessor.componentType != TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT), "TinyGlTF Error: Mesh Doesnt Have uint32_t Indices");

				const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
				for (size_t index = 0; index < accessor.count; index++)
				{
					subdata.indices.push_back(buf[index]);
				}

			}
		}
	}

	for (uint32_t i = 0; i < (uint32_t)input.images.size(); i++)
	{
		data->imageData[i] = input.images[i].uri;
	}

	for (uint32_t i = 0; i < (uint32_t)input.materials.size(); i++)
	{
		auto& material = input.materials[i];

		if (material.values.find("baseColorTexture") != material.values.end())
		{
			uint32_t textureIndex = input.materials[i].values["baseColorTexture"].TextureIndex();
			int texture = input.textures[textureIndex].source;
	
			data->materialData[i].AlbedoTextureIndex = texture;
		}

	}

	return true;
}

Nexus::Ref<Nexus::StaticMesh> Nexus::StaticMesh::Create(const std::string& filepath)
{
	MeshData data;

	Ref<StaticMesh> mesh = CreateRef<StaticMesh>();

	if (!LoadGLTF(filepath, &data))
	{
		mesh->m_Id = NullUUID;
		NEXUS_LOG_ERROR("Mesh Loading Error");
		return nullptr;
	}
	
	NEXUS_LOG_TRACE("Mesh Loaded: {0}", filepath);

	uint32_t numSubmesh = (uint32_t)data.submeshData.size();

	mesh->m_SubMeshes.resize(numSubmesh);
	for (uint32_t i = 0; i < numSubmesh; i++)
	{
		mesh->m_SubMeshes[i].vb = StaticBuffer::Create((uint32_t)data.submeshData[i].vertices.size() * sizeof(StaticMeshVertex), BufferType::Vertex, data.submeshData[i].vertices.data());
		mesh->m_SubMeshes[i].ib = StaticBuffer::Create((uint32_t)data.submeshData[i].indices.size() * sizeof(uint32_t), BufferType::Index, data.submeshData[i].indices.data());
	}

	return mesh;
}
