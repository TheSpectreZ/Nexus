#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "NxCore/UUID.h"
#include "NxCore/Logger.h"
#include "NxCore/Assertion.h"

#include "NxAsset/Asset.h"

#include "NxScene/Scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "nlohmann/json.hpp"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_IMPLEMENTATION
#include "tinyglTF/tiny_gltf.h"

static std::string MeshExtension = ".NxMesh";
static std::string TextureExtension = ".NxTex";
static std::string MaterialExtension = ".NxMat";
static std::string AnimExtension = ".NxAnim";

static std::string BinExtension = ".NxBin";

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

	Nexus::Meshing::Node* FindNode(Nexus::Meshing::Node* parent, uint32_t Index)
	{
		Nexus::Meshing::Node* nodeFound = nullptr;
		if (parent->Index == Index)
			return parent;
		for (auto& child : parent->children)
		{
			nodeFound = FindNode(child, Index);
			if (nodeFound)
				break;
		}
		return nodeFound;
	}

	Nexus::Meshing::Node* FindNodeFromStack(std::vector<Nexus::Meshing::Node>& nodes, uint32_t index)
	{
		Nexus::Meshing::Node* nodeFound = nullptr;
		for (auto& node : nodes) {
			nodeFound = FindNode(&node, index);
			if (nodeFound) {
				break;
			}
		}
		return nodeFound;
	}

	bool LoadSceneFromFile(const std::filesystem::path& Filepath, Meshing::Scene* data)
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

		// Mesh
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

			data->mesh.vertices.reserve(totalVertices);
			data->mesh.indices.reserve(totalIndices);

			for (auto& [k, vert] : vertices)
			{
				auto& sm = data->mesh.submeshes.emplace_back();

				sm.indexOffset = (uint32_t)data->mesh.indices.size();
				sm.indexSize = (uint32_t)indices[k].size();
				sm.materialIndex = k;

				data->mesh.vertices.insert(data->mesh.vertices.end(), vert.begin(), vert.end());
				data->mesh.indices.insert(data->mesh.indices.end(), indices[k].begin(), indices[k].end());
			}
		}

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
			m.textureCoords.albedo = material.pbrMetallicRoughness.baseColorTexture.texCoord;

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
					m.textureCoords.albedo = dft.Get("texCoord").Get<int>();
				}
				if (sg.Has("diffuseFactor"))
				{
					auto& df = sg.Get("diffuseFactor");
					for (uint32_t i = 0; i < df.ArrayLen(); i++)
						m.specularGlossiness.aldeboColor[i] = df.Get(i).IsNumber() ? (float)df.Get(i).Get<double>() : (float)df.Get(i).Get<int>();
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

		// Nodes
		{
			data->nodes.resize(scene.nodes.size());
			for (uint32_t i = 0; i < (uint32_t)scene.nodes.size(); i++)
			{
				auto& glNode = scene.nodes[i];
				auto& mNode = data->nodes[i];

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

					auto& child = data->nodes[cIndex];
					child.parent = &mNode;

					mNode.children[j] = &child;
				}
			}
		}

		// Skins
		for (auto& skin : scene.skins)
		{
			auto& s = data->skins.emplace_back();
			s.Name = skin.name;
			s.skeletonRoot = FindNodeFromStack(data->nodes, skin.skeleton);

			s.Joints.resize(skin.joints.size());
			for (auto i = 0; i < s.Joints.size(); i++)
			{
				s.Joints[i] = FindNodeFromStack(data->nodes, skin.joints[i]);
			}
		}

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
				ac.node = FindNodeFromStack(data->nodes, chan.target_node);

				if (!ac.node)
					continue;
			}
		}

		return true;
	}

}

namespace Nexus::Utils
{
	bool ImportMesh(const AssetFilePath& dstFolder,Meshing::Mesh& mesh,const std::string& name, const std::unordered_map<uint32_t, std::string>& materialTable)
	{
		nlohmann::json Json;
		Json["UUID"] = (uint64_t)UUID();
		Json["AssetType"] = "Mesh";
		
		nlohmann::json Mesh;
		Mesh["VertexCount"] = mesh.vertices.size();
		Mesh["IndexCount"] = mesh.indices.size();
		Mesh["SubmeshCount"] = mesh.submeshes.size();

		Json["Meta"] = Mesh;

		if (!materialTable.empty())
		{
			nlohmann::json MaterialTable;
			for (auto& [k, v] : materialTable)
				MaterialTable[std::to_string(k)] = v;
			
			Json["Material_Table"] = MaterialTable;
		}
		
		std::string JsonDump = Json.dump(4);

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + name + MeshExtension);
		
		std::ofstream stream(file, std::ios::binary);
		if (!stream.is_open())
			return false;

		uint32_t size = JsonDump.length();
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

		uint32_t size = JsonDump.length();
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

		uint32_t size = JsonDump.length();
		stream.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		stream.write(JsonDump.c_str(), JsonDump.length());

		stream.write(reinterpret_cast<const char*>(texture->image.pixels.data()), texture->image.pixels.size() * sizeof(uint8_t));

		stream.close();

		return true;
	}

	std::string ImportMaterial(const AssetFilePath& dstFolder, const Meshing::Material* material, const std::vector<Meshing::Texture>& textures)
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
			Name = std::to_string(uid.operator size_t());

		AssetFilePath file = AssetFilePath(dstFolder.generic_string() + "/" + Name + MaterialExtension);

		if (std::filesystem::exists(file))
			file.replace_filename(std::string(Name + "- Copy" + MaterialExtension));

		std::ofstream stream(file);
		stream << JsonDump;
		
		return file.generic_string();
	}
}

bool Nexus::Importer::ImportGLTF(const AssetFilePath& path, const AssetFilePath& dstFolder,const std::string& Name)
{
	Meshing::Scene scene;
	if (!Nexus::glTF::LoadSceneFromFile(path, &scene))
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

	std::unordered_map<uint32_t, std::string> materialTable;
	uint32_t i = 0;
	for (auto& Mat : scene.materials)
	{
		auto path = Utils::ImportMaterial(MatFolder, &Mat, scene.textures);
		materialTable[i++] = path;
	}

	Utils::ImportMesh(dstFolder, scene.mesh, Name, materialTable);

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

std::pair<bool, Nexus::UUID> Nexus::Importer::LoadMesh(const AssetFilePath& path, Meshing::Mesh& mesh, std::unordered_map<uint32_t, std::string>* materialTablePtr)
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

		if (materialTablePtr && Json.contains("Material_Table"))
		{
			auto& table = Json["Material_Table"];

			for (const auto& entry : table.items())
			{
				uint32_t key = std::stoul(entry.key());
				std::string value = entry.value();
				materialTablePtr->emplace(key, value);
			}
		}

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

std::pair<bool, Nexus::UUID> Nexus::Importer::Loadimage(const Nexus::AssetFilePath& path,Nexus::Meshing::Image& image)
{
	Nexus::UUID Id(true);

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
		auto [res, Id] = LoadTexture(Path, textures[2]);

		if (res)
			material.specularGlossiness.specularGlossinessTexture = UUID(Id);
	}

	return { true,UId };
}
