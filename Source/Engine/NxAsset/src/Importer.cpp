#include "NxAsset/Importer.h"

#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "nlohmann/json.hpp"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_IMPLEMENTATION
#include "tinyglTF/tiny_gltf.h"

#include "NxCore/Registry.h"

namespace Nexus::glTF
{
	using namespace Nexus::Meshing;

	void CalculateTangentAndBiTangent(Nexus::Meshing::Vertex& v0, Nexus::Meshing::Vertex& v1, Nexus::Meshing::Vertex& v2, bool foundTangent, bool foundBiTangent)
	{
		glm::vec3 deltaPos1 = v1.position - v0.position;
		glm::vec3 deltaPos2 = v2.position - v0.position;

		glm::vec2 deltaUV1 = v1.texCoord0 - v0.texCoord0;
		glm::vec2 deltaUV2 = v2.texCoord0 - v0.texCoord0;

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
			NEXUS_LOG("glTF Importer Error", "Unknown GLTF Filter Mode: %i", filter);
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
			NEXUS_LOG("glTF Importer Error", "Unknown GLTF Wrap Mode: {0}", mode);
			return Nexus::SamplerWrapMode::Repeat;
		}
	}

#define FIND_VALUE_BY_NAME(data,name) data.values.find(name) != data.values.end()

	Nexus::Meshing::Node* FindNode(std::vector<Nexus::Meshing::Node>& nodes,Nexus::Meshing::Node* parent, uint32_t Index)
	{
		Nexus::Meshing::Node* nodeFound = nullptr;
		if (parent->Index == Index)
			return parent;
		for (auto& child : parent->children)
		{
			nodeFound = FindNode(nodes, &nodes[child], Index);
			if (nodeFound)
				break;
		}
		return nodeFound;
	}

	Nexus::Meshing::Node* FindNodeFromStack(std::vector<Nexus::Meshing::Node>& nodes, uint32_t index)
	{
		Nexus::Meshing::Node* nodeFound = nullptr;
		for (auto& node : nodes) {
			nodeFound = FindNode(nodes, &node, index);
			if (nodeFound) {
				break;
			}
		}
		return nodeFound;
	}

	bool LoadSceneFromFile(const std::filesystem::path& Filepath, Meshing::Scene* data,const glTFImportSettings& settings)
	{
		tinygltf::Model scene;

		// Import
		{
			tinygltf::TinyGLTF Importer;

			std::string error, warning;

			bool glbFile = false;

			if (Filepath.extension().string() == ".glb")
				glbFile = true;
			else if (Filepath.extension().string() != ".gltf")
			{
				data = nullptr;
				NEXUS_LOG("glTF Importer", "Incorrect File Extentsion - &s", Filepath.generic_string().c_str());
				return false;
			}

			bool success = glbFile ?
				Importer.LoadBinaryFromFile(&scene, &error, &warning, Filepath.string()) :
				Importer.LoadASCIIFromFile(&scene, &error, &warning, Filepath.string());

			if (!warning.empty())
			{
				NEXUS_LOG("glTF Importer", "Warning - %s", warning.c_str());
			}

			if (!error.empty())
			{
				NEXUS_LOG("glTF Importer", "Error - %s", error.c_str());
			}

			if (!success)
			{
				NEXUS_LOG("glTF Importer", "Failed to Load File - %s", Filepath.string().c_str());
				return false;
			}
		}

		if (settings.loadSkeleton)
		{
			// Meshes
			for (auto& mesh : scene.meshes)
			{
				size_t totalVertices = 0;
				size_t totalIndices = 0;

				std::unordered_map<uint32_t, std::vector<Vertex>> vertices;
				std::unordered_map<uint32_t, std::vector<uint32_t>> indices;

				for (auto& primitive : mesh.primitives)
				{
					bool foundTangent = false;
					bool foundBiTangent = false;

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

					auto& smV = vertices[primitive.material];

					auto prevSize = smV.size();
					smV.resize(prevSize + vertexCount);

					for (size_t i = 0; i < vertexCount; i++)
					{
						auto& vertex = smV[prevSize + i];

						vertex.position = glm::make_vec3(&positionBuffer[i * 3]);

						if (normalBuffer)
							vertex.normal = glm::make_vec3(&normalBuffer[i * 3]);
						else
							vertex.normal = glm::vec3(0.f);

						if (texCoordBuffer)
							vertex.texCoord0 = glm::make_vec2(&texCoordBuffer[i * 2]);
						else
							vertex.texCoord0 = glm::vec2(0.f);

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

					auto& smI = indices[primitive.material];
					prevSize = smI.size();
					smI.resize(prevSize + indexCount);

					if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index += 3)
						{
							int a = buf[index];
							int b = buf[index + 1];
							int c = buf[index + 2];

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
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

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
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

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
							}
						}
					}
					else
					{
						auto err = "GLTF Indices Type Not Supported" + accessor.componentType;
						NEXUS_ASSERT(true, err);
					}

					totalVertices += vertexCount;
					totalIndices += indexCount;
				}

				auto& Mesh = data->meshSource.meshes.emplace_back();

				Mesh.vertices.reserve(totalVertices);
				Mesh.indices.reserve(totalIndices);

				for (auto& [k, vert] : vertices)
				{
					auto& sm = Mesh.submeshes.emplace_back();

					sm.indexOffset = (uint32_t)Mesh.indices.size();
					sm.indexSize = (uint32_t)indices[k].size();
					sm.materialIndex = k;

					Mesh.vertices.insert(Mesh.vertices.end(), vert.begin(), vert.end());
					Mesh.indices.insert(Mesh.indices.end(), indices[k].begin(), indices[k].end());
				}
			}
			
			// Nodes
			data->skeleton.nodes.resize(scene.nodes.size());
			for (uint32_t i = 0; i < (uint32_t)scene.nodes.size(); i++)
			{
				auto& glNode = scene.nodes[i];
				auto& mNode = data->skeleton.nodes[i];

				mNode.name = glNode.name;
				mNode.Index = i;

				if (!glNode.matrix.empty())
					mNode.matrix = glm::make_mat4(glNode.matrix.data());

				if (!glNode.translation.empty())
					mNode.translation = glm::make_vec3(glNode.translation.data());

				if (!glNode.scale.empty())
					mNode.scale = glm::make_vec3(glNode.scale.data());

				if (!glNode.rotation.empty())
					mNode.rotation = glm::make_quat(glNode.rotation.data());

				mNode.mesh = (uint32_t)glNode.mesh;
				mNode.skin = glNode.skin;

				mNode.children.resize(glNode.children.size());
				for (uint32_t j = 0; j < (uint32_t)glNode.children.size(); j++)
				{
					auto cIndex = glNode.children[j];

					auto& child = data->skeleton.nodes[cIndex];
					child.parent = i;

					mNode.children[j] = cIndex;
				}
			}

			// Skins
			for (auto& skin : scene.skins)
			{
				auto& s = data->skeleton.skins.emplace_back();
				s.Name = skin.name;
				s.skeletonRoot = FindNodeFromStack(data->skeleton.nodes, skin.skeleton);

				s.Joints.resize(skin.joints.size());
				for (auto i = 0; i < s.Joints.size(); i++)
				{
					s.Joints[i] = FindNodeFromStack(data->skeleton.nodes, skin.joints[i]);
				}
			}
		}
		else
		{
			size_t totalVertices = 0;
			size_t totalIndices = 0;

			std::unordered_map<uint32_t, std::vector<Vertex>> vertices;
			std::unordered_map<uint32_t, std::vector<uint32_t>> indices;

			for (auto& mesh : scene.meshes)
			{
				for (auto& primitive : mesh.primitives)
				{
					bool foundTangent = false;
					bool foundBiTangent = false;

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

					auto& smV = vertices[primitive.material];

					auto prevSize = smV.size();
					smV.resize(prevSize + vertexCount);

					for (size_t i = 0; i < vertexCount; i++)
					{
						auto& vertex = smV[prevSize + i];

						vertex.position = glm::make_vec3(&positionBuffer[i * 3]);

						if (normalBuffer)
							vertex.normal = glm::make_vec3(&normalBuffer[i * 3]);
						else
							vertex.normal = glm::vec3(0.f);

						if (texCoordBuffer)
							vertex.texCoord0 = glm::make_vec2(&texCoordBuffer[i * 2]);
						else
							vertex.texCoord0 = glm::vec2(0.f);

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

					auto& smI = indices[primitive.material];
					prevSize = smI.size();
					smI.resize(prevSize + indexCount);

					if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
						for (size_t index = 0; index < accessor.count; index += 3)
						{
							int a = buf[index];
							int b = buf[index + 1];
							int c = buf[index + 2];

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
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

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
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

							smI[prevSize + index] = (uint32_t)totalVertices + a;
							smI[prevSize + index + 1] = (uint32_t)totalVertices + b;
							smI[prevSize + index + 2] = (uint32_t)totalVertices + c;

							if (!foundTangent || !foundBiTangent)
							{
								CalculateTangentAndBiTangent(smV[a], smV[b], smV[c], foundTangent, foundBiTangent);
							}
						}
					}
					else
					{
						auto err = "GLTF Indices Type Not Supported" + accessor.componentType;
						NEXUS_ASSERT(true, err);
					}

					totalVertices += vertexCount;
					totalIndices += indexCount;
				}
			}

			auto& Mesh = data->meshSource.meshes.emplace_back();

			Mesh.vertices.reserve(totalVertices);
			Mesh.indices.reserve(totalIndices);

			for (auto& [k, vert] : vertices)
				{
					auto& sm = Mesh.submeshes.emplace_back();

					sm.indexOffset = (uint32_t)Mesh.indices.size();
					sm.indexSize = (uint32_t)indices[k].size();
					sm.materialIndex = k;

					Mesh.vertices.insert(Mesh.vertices.end(), vert.begin(), vert.end());
					Mesh.indices.insert(Mesh.indices.end(), indices[k].begin(), indices[k].end());
				}
		}
		
		if (settings.loadMaterials)
		{
			// Textures
			for (auto& texture : scene.textures)
			{
				auto& t = data->textures.emplace_back();

				auto& image = scene.images[texture.source];
				{
					std::filesystem::path path = Filepath.parent_path().generic_string() + "/" + image.uri;

					int w, h, c;
					auto pixels = stbi_load(path.generic_string().c_str(), &w, &h, &c, 4);

					t.image.height = h;
					t.image.width = w;
					t.image.channels = 4;
					t.image.fileName = path.filename().stem().string();

					t.image.pixels.resize((size_t)w * h * 4);
					memcpy(t.image.pixels.data(), pixels, t.image.pixels.size() * sizeof(uint8_t));

					stbi_image_free(pixels);
				}

				auto& sampler = scene.samplers[texture.sampler];
				{
					Sampler s{};
					s.Near = GetFilterMode(sampler.magFilter);
					s.Far = GetFilterMode(sampler.minFilter);
					s.U = GetWrapMode(sampler.wrapS);
					s.V = GetWrapMode(sampler.wrapT);
					s.W = s.V;

					t.samplerHash = s.GetHash();
				}
			}

			// Materials
			for (auto& material : scene.materials)
			{
				auto& m = data->materials.emplace_back();
				m.Name = material.name;

				m.metalicRoughness.albedoColor = glm::make_vec4(material.pbrMetallicRoughness.baseColorFactor.data());
				m.metalicRoughness.metallic = (float)material.pbrMetallicRoughness.metallicFactor;
				m.metalicRoughness.roughness = (float)material.pbrMetallicRoughness.roughnessFactor;

				m.metalicRoughness.albedoTexture = material.pbrMetallicRoughness.baseColorTexture.index;
				m.textureCoords.baseColor = material.pbrMetallicRoughness.baseColorTexture.texCoord;

				m.metalicRoughness.metallicRoughnessTexture = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
				m.textureCoords.metallicRoughness = material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

				m.normalTexture = material.normalTexture.index;
				m.textureCoords.normal = material.normalTexture.texCoord;

				m.occulsionTexture = material.occlusionTexture.index;
				m.textureCoords.occlusion = material.occlusionTexture.texCoord;

				m.emissiveTexture = material.emissiveTexture.index;
				m.textureCoords.emissive = material.emissiveTexture.texCoord;
				m.emissiveColor = glm::make_vec3(material.emissiveFactor.data());

				if (material.extensions.contains("KHR_materials_pbrSpecularGlossiness"))
				{
					auto& sg = material.extensions["KHR_materials_pbrSpecularGlossiness"];

					if (sg.Has("specularGlossinessTexture"))
					{
						auto& sgt = sg.Get("specularGlossinessTexture");

						m.specularGlossiness.support = true;
						m.specularGlossiness.specularGlossinessTexture = sgt.Get("index").Get<int>();
						m.textureCoords.specularGlossiness = sgt.Get("texCoord").Get<int>();
					}
					if (sg.Has("diffuseTexture"))
					{
						auto& dft = sg.Get("diffuseTexture");

						m.specularGlossiness.albedoTexture = dft.Get("index").Get<int>();
						m.textureCoords.baseColor = dft.Get("texCoord").Get<int>();
					}
					if (sg.Has("diffuseFactor"))
					{
						auto& df = sg.Get("diffuseFactor");
						for (uint32_t i = 0; i < df.ArrayLen(); i++)
							m.specularGlossiness.albedoColor[i] = df.Get(i).IsNumber() ? (float)df.Get(i).Get<double>() : (float)df.Get(i).Get<int>();
					}
					if (sg.Has("specularFactor"))
					{
						auto& sf = sg.Get("specularFactor");
						for (uint32_t i = 0; i < sf.ArrayLen(); i++)
							m.specularGlossiness.specular[i] = sf.Get(i).IsNumber() ? (float)sf.Get(i).Get<double>() : (float)sf.Get(i).Get<int>();
					}
					if (sg.Has("glossinessFactor"))
					{
						m.specularGlossiness.glossiness = (float)sg.Get("glossinessFactor").Get<double>();
					}
				}
			}
		}

		if (settings.loadAnimations)
		{
			// Animations
			for (auto& animation : scene.animations)
			{
				auto& a = data->animations.emplace_back();
				a.Name = animation.name;

				for (auto& samp : animation.samplers)
				{
					auto& as = a.samplers.emplace_back();

					if (samp.interpolation == "LINEAR")
						as.type = Animation::Sampler::InterpolationType::Linear;

					if (samp.interpolation == "STEP")
						as.type = Animation::Sampler::InterpolationType::Step;

					if (samp.interpolation == "CUBICSPLINE")
						as.type = Animation::Sampler::InterpolationType::CubicSpline;

					// Read sampler input time values
					{
						const tinygltf::Accessor& accessor = scene.accessors[samp.input];
						const tinygltf::BufferView& bufferView = scene.bufferViews[accessor.bufferView];
						const tinygltf::Buffer& buffer = scene.buffers[bufferView.buffer];

						assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

						const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
						const float* buf = static_cast<const float*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							as.input.push_back(buf[index]);
						}

						for (auto input : as.input)
						{
							if (input < a.start)
							{
								a.start = input;
							};

							if (input > a.end)
							{
								a.end = input;
							}
						}
					}

					// Read sampler output T/R/S values 
					{
						const tinygltf::Accessor& accessor = scene.accessors[samp.output];
						const tinygltf::BufferView& bufferView = scene.bufferViews[accessor.bufferView];
						const tinygltf::Buffer& buffer = scene.buffers[bufferView.buffer];

						assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

						const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

						switch (accessor.type)
						{
						case TINYGLTF_TYPE_VEC3:
						{
							const glm::vec3* buf = static_cast<const glm::vec3*>(dataPtr);
							for (size_t index = 0; index < accessor.count; index++)
							{
								as.output.push_back(glm::vec4(buf[index], 0.0f));
							}
							break;
						}
						case TINYGLTF_TYPE_VEC4:
						{
							const glm::vec4* buf = static_cast<const glm::vec4*>(dataPtr);
							for (size_t index = 0; index < accessor.count; index++)
							{
								as.output.push_back(buf[index]);
							}
							break;
						}
						default:
							break;
						}
					}

				}

				// Channels
				for (auto& chan : animation.channels)
				{
					auto& ac = a.channels.emplace_back();

					if (chan.target_path == "rotation")
						ac.type = Animation::Channel::PathType::Rotation;
					if (chan.target_path == "translation")
						ac.type = Animation::Channel::PathType::Translation;
					if (chan.target_path == "scale")
						ac.type = Animation::Channel::PathType::Scale;

					if (chan.target_path == "weights")
					{
						NEXUS_LOG("Meshing", "Skipping Weights Anim Channel");
						continue;
					}

					ac.samplerIndex = chan.sampler;
					ac.node = FindNodeFromStack(data->skeleton.nodes, chan.target_node)->Index;

					if (!ac.node)
						continue;
				}
			}
		}

		return true;
	}
}

namespace stdfs = std::filesystem;
#define CREATE_NON_EXISTING_DIRECTORY(path) if(!stdfs::exists(path)) stdfs::create_directories(path)

#define CREATE_UNIQUE_FPATH(Dir,name,Extension) std::string Name = name + Extension;\
AssetFilePath fPath = Dir / Name;\
if (stdfs::exists(fPath))\
{\
	Name = name + "-Copy" + Extension;\
	fPath.replace_filename(Name);\
}

#define REGISTER_TEXTURE_IN_MATERIAL(id) if(id != UINT64_MAX) id = Uids[(uint32_t)id]
	 
bool Nexus::Importer::ImportglTF(const AssetFilePath& filepath, const glTFImportSettings& settings)
{
	Meshing::Scene scene;
	if (!glTF::LoadSceneFromFile(filepath, &scene, settings))
		return false;

	CREATE_NON_EXISTING_DIRECTORY(settings.path);

	auto registry = AssetRegistry::Get();

	std::vector<UUID> matIds;

	if (!scene.materials.empty())
	{
		std::unordered_map<uint32_t, UUID> Uids;

		if (!scene.textures.empty())
		{
			AssetFilePath dir = settings.path / "Textures";
			CREATE_NON_EXISTING_DIRECTORY(dir);

			uint32_t i = 0;
			for (auto& tex : scene.textures)
			{
				CREATE_UNIQUE_FPATH(dir, tex.image.fileName, ".NxTex")
				tex.Serialize(fPath);

				Uids[i++] = registry->Register(fPath);
			}
		}

		AssetFilePath dir = settings.path / "Materials";
		CREATE_NON_EXISTING_DIRECTORY(dir);

		for (auto& mat : scene.materials)
		{
			// registering Ids
			{
				REGISTER_TEXTURE_IN_MATERIAL(mat.metalicRoughness.albedoTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.metalicRoughness.metallicRoughnessTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.specularGlossiness.albedoTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.specularGlossiness.specularGlossinessTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.normalTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.emissiveTexture);
				REGISTER_TEXTURE_IN_MATERIAL(mat.occulsionTexture);
			}

			CREATE_UNIQUE_FPATH(dir, mat.Name, ".NxMat")
			mat.Serialize(fPath);

			matIds.push_back(registry->Register(fPath));
		}
	}

	if (!scene.animations.empty())
	{
		AssetFilePath dir = settings.path / "Animmations";
		CREATE_NON_EXISTING_DIRECTORY(dir);

		// Making Animation Files
		for (auto& a : scene.animations)
		{
			CREATE_UNIQUE_FPATH(dir, a.Name, ".NxAnim");

			a.Serialize(fPath);
			registry->Register(fPath);
		}
	}
	
	UUID skel(true);
	
	AssetFilePath mdir = settings.path / "Mesh";
	CREATE_NON_EXISTING_DIRECTORY(mdir);

	if (!scene.skeleton.nodes.empty())
	{
		CREATE_UNIQUE_FPATH(mdir, settings.Name, ".NxSkeleton");

		scene.skeleton.Serialize(fPath);
		skel = registry->Register(fPath);
	}

	// Mesh
	{
		CREATE_UNIQUE_FPATH(mdir, settings.Name, ".NxMeshSource");

		scene.meshSource.Serialize(fPath);
		UUID Id = registry->Register(fPath);

		nlohmann::json Json;
		{
			Json["MeshSource"] = Id.operator size_t();

			if (settings.loadMaterials)
			{
				nlohmann::json mat;
				for (uint32_t i = 0; i < matIds.size(); i++)
					mat[std::to_string(i)] = matIds[i].operator size_t();

				Json["MaterialTable"] = mat;
			}

			if (settings.loadSkeleton)
			{
				Json["Skeleton"] = skel.operator size_t();
			}
		}

		Name = settings.Name + ".NxMeshAsset"; 
		fPath = settings.path / Name; 
		if (stdfs::exists(fPath))
			Name = settings.Name + "-Copy" + ".NxMeshAsset"; fPath.replace_filename(Name);

		std::ofstream stream(fPath);

		std::string JsonDump = Json.dump(4);
		stream << JsonDump;
	}

	return true;
}

bool Nexus::Importer::ImportImage(const AssetFilePath& filepath, const std::string& dstFolder, uint32_t samplerHash)
{
	int w, h, c;
	auto pixels = stbi_load(filepath.generic_string().c_str(), &w, &h, &c, 4);
	if (!pixels)
		return false;

	Meshing::Texture tex;

	auto filename = filepath.filename().stem();

	tex.image.height = h;
	tex.image.width = w;
	tex.image.channels = 4;
	tex.image.fileName = filepath.filename().stem().generic_string();

	tex.image.pixels.resize((size_t)w * h * 4);
	memcpy(tex.image.pixels.data(), pixels, tex.image.pixels.size() * sizeof(uint8_t));

	stbi_image_free(pixels);

	tex.samplerHash = samplerHash;

	AssetFilePath dir = dstFolder;
	CREATE_UNIQUE_FPATH(dir, tex.image.fileName, ".NxTex")
	tex.Serialize(fPath);

	AssetRegistry::Get()->Register(fPath);

	return true;
}

void Nexus::Importer::Mat()
{
	Meshing::Material mat;
	mat.Name = "Default";
	mat.metalicRoughness.roughness = 0.5f;
	mat.metalicRoughness.metallic = 0.5f;

	AssetFilePath dir = "Resources/Material";
	CREATE_UNIQUE_FPATH(dir, mat.Name, ".NxMat")
	mat.Serialize(fPath);

	AssetRegistry::Get()->Register(fPath);
}

bool Nexus::Loader::LoadMeshAsset(const AssetFilePath& filepath, MeshAssetSpecification* specs)
{
	std::ifstream stream(filepath);
	if (!stream.is_open())
		return false;

	std::stringstream ss;
	ss << stream.rdbuf();

	nlohmann::json Json = nlohmann::json::parse(ss.str());

	specs->mesh = Json["MeshSource"].get<uint64_t>();

	if (Json.contains("Skeleton"))
		specs->skeleton = Json["Skeleton"].get<uint64_t>();

	if (Json.contains("MaterialTable"))
	{
		auto& mt = Json["MaterialTable"];
		for (auto& item : mt.items())
		{
			uint32_t key = std::stoul(item.key());
			uint64_t value = item.value();

			specs->materials[key] = value;
		}
	}

	stream.close();
	return true;
}
