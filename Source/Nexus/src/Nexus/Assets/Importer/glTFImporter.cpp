#include "nxpch.h"
#include "glTFImporter.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include <glm/gtc/type_ptr.hpp>

Nexus::SamplerFilter GetFilterMode(int32_t filter)
{
	switch (filter) 
	{
	case 9728:
		return Nexus::SamplerFilter::Nearest;
	case 9729:
		return Nexus::SamplerFilter::Linear;
	case 9984:
		return Nexus::SamplerFilter::Nearest;
	case 9985:
		return Nexus::SamplerFilter::Nearest;
	case 9986:
		return Nexus::SamplerFilter::Linear;
	case 9987:
		return Nexus::SamplerFilter::Linear;
	default:
		NEXUS_LOG_ERROR("Unknown GLTF Filter Mode: {0}", filter);
		return Nexus::SamplerFilter::Linear;
	}
}

Nexus::SamplerWrapMode GetWrapMode(int32_t mode)
{
	switch (mode)
	{
	case 10497:
		return Nexus::SamplerWrapMode::Repeat;
	case 33071:
		return Nexus::SamplerWrapMode::Clamped_To_Edge;
	case 33648:
		return Nexus::SamplerWrapMode::Mirrored_Repeat;
	default:
		NEXUS_LOG_ERROR("Unknown GLTF Wrap Mode: {0}", mode);
		return Nexus::SamplerWrapMode::Repeat;
	}
}

#define FIND_VALUE_BY_NAME(data,name) data.values.find(name) != data.values.end()

bool Nexus::Importer::glTF::Load(const std::filesystem::path& Filepath, glTFSceneData* data)
{
	tinygltf::Model scene;
	tinygltf::TinyGLTF Importer;

	// Importing glTF
	{
		std::string error, warning;

		bool glbFile = false;

		if (Filepath.extension().string() == ".glb")
			glbFile = true;
		else if (Filepath.extension().string() != ".gltf")
		{
			data = nullptr;
			NEXUS_LOG_ERROR("AssetImporter::glTF - Incorrect File Extentsion");
			return false;
		}

		bool success = glbFile ? Importer.LoadBinaryFromFile(&scene, &error, &warning, Filepath.string()) : Importer.LoadASCIIFromFile(&scene, &error, &warning, Filepath.string());

		if (!warning.empty())
		{
			NEXUS_LOG_ERROR("AssetImporter::glTF Warning - {0}", warning);
		}

		if (!error.empty())
		{
			NEXUS_LOG_ERROR("AssetImporter::glTF Error - {0}", error);
		}

		if (!success)
		{
			NEXUS_LOG_ERROR("AssetImporter::glTF Failed to Load File - {0}", Filepath.string());
			return false;
		}
	}

	// Material
	{
		for (auto& image : scene.images)
		{
			auto& i = data->images.emplace_back();
			i.sourcePath = image.uri;
		}

		for (auto& sampler : scene.samplers)
		{
			auto& s = data->samplers.emplace_back();
			
			s.Near = GetFilterMode(sampler.magFilter);
			s.Far = GetFilterMode(sampler.minFilter);
			s.U = GetWrapMode(sampler.wrapS);
			s.V = GetWrapMode(sampler.wrapT);
			s.W = s.V;
		}

		for (auto& texture : scene.textures)
		{
			auto& t = data->textures.emplace_back();

			t.Image = texture.source;
			t.Sampler = texture.sampler;
		}

		for (auto& material : scene.materials)
		{
			auto& m = data->materials.emplace_back();

			if (FIND_VALUE_BY_NAME(material, "baseColorTexture"))
			{
				m.albedoTexture = material.values["baseColorTexture"].TextureIndex();
				m.textureCoords.albedo = material.values["baseColorTexture"].TextureTexCoord();
			}

			if (FIND_VALUE_BY_NAME(material, "metallicRoughnessTexture"))
			{
				m.metallicRoughnessTexture = material.values["metallicRoughnessTexture"].TextureIndex();
				m.textureCoords.metallicRoughness = material.values["metallicRoughnessTexture"].TextureTexCoord();
			}

			if (FIND_VALUE_BY_NAME(material, "normalTexture"))
			{
				m.normalTexture = material.values["normalTexture"].TextureIndex();
				m.textureCoords.normal = material.values["normalTexture"].TextureTexCoord();
			}

			if (FIND_VALUE_BY_NAME(material, "baseColorFactor"))
			{
				m.albedoColor = glm::make_vec4(material.values["baseColorFactor"].ColorFactor().data());
			}

			if (FIND_VALUE_BY_NAME(material, "roughnessFactor"))
			{
				m.roughness = (float)material.values["roughnessFactor"].Factor();
			}

			if (FIND_VALUE_BY_NAME(material, "metallicFactor"))
			{
				m.metallic = (float)material.values["metallicFactor"].Factor();
			}
		}
	}

	// Mesh
	{
		for (auto& mesh : scene.meshes)
		{
			auto& myMesh = data->meshes.emplace_back();

			for (auto& primitive : mesh.primitives)
			{
				auto& submesh = myMesh.submeshes.emplace_back();
				
				submesh.materialIndex = primitive.material;

				// Vertices
				{
					const float* positionBuffer = nullptr;
					const float* normalBuffer = nullptr;
					const float* texCoord0Buffer = nullptr;
					const float* texCoord1Buffer = nullptr;
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

					auto t0 = primitive.attributes.find("TEXCOORD_0");
					if (t0 != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[t0->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						texCoord0Buffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
					}

					auto t1 = primitive.attributes.find("TEXCOORD_1");
					if (t1 != primitive.attributes.end())
					{
						auto& accessor = scene.accessors[t1->second];
						auto& view = scene.bufferViews[accessor.bufferView];
						texCoord1Buffer = reinterpret_cast<const float*>(&scene.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
					}

					submesh.vertices.resize(vertexCount);
					for (size_t i = 0; i < vertexCount; i++)
					{
						auto& vertex = submesh.vertices[i];

						vertex.position = glm::make_vec3(&positionBuffer[i * 3]);

						if (normalBuffer)
							vertex.normal = glm::make_vec3(&normalBuffer[i * 3]);
						else
							vertex.normal = glm::vec3(0.f);

						if (texCoord0Buffer)
							vertex.texCoord0 = glm::make_vec2(&texCoord0Buffer[i * 2]);
						else
							vertex.texCoord0 = glm::vec2(0.f);

						if (texCoord1Buffer)
							vertex.texCoord1 = glm::make_vec2(&texCoord1Buffer[i * 2]);
						else
							vertex.texCoord1 = glm::vec2(0.f);
					}
				}

				// Indices
				{
					auto& accessor = scene.accessors[primitive.indices];
					auto& view = scene.bufferViews[accessor.bufferView];
					auto& buffer = scene.buffers[view.buffer];

					uint32_t indexCount = (uint32_t)accessor.count;

					if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							submesh.indices.push_back(buf[index]);
						}
					}
					else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
					{
						const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							submesh.indices.push_back(buf[index]);
						}
					}
					else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
					{
						const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							submesh.indices.push_back(buf[index]);
						}
					}
					else
					{
						auto err = "GLTF Indices Type Not Supported" + accessor.componentType;
						NEXUS_ASSERT(true, err);
					}
				}

			}
		}
	}

	NEXUS_LOG_TRACE("AssetImporter::glTF Successfully Loaded File - {0}", Filepath.string());
	return true;
}
