#pragma once
#include <filesystem>
#include "glm/glm.hpp"

#include "Renderer/Vertex.h"
#include "Renderer/RenderTypes.h"

namespace Nexus::Importer::glTF
{
	struct Submesh
	{
		std::vector<StaticMeshVertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;
	};

	struct Mesh
	{
		std::vector<Submesh> submeshes;
	};

	struct Image
	{
		std::string sourcePath;
	};
	
	struct Sampler
	{
		SamplerFilter Near, Far;
		SamplerWrapMode U, V, W;
	};

	struct Texture
	{
		uint32_t Image;
		uint32_t Sampler;
	};

	struct Material
	{
		struct TexCoords
		{
			uint8_t albedo;
			uint8_t metallicRoughness;
			uint8_t normal;
		} textureCoords;

		uint32_t albedoTexture = UINT32_MAX;
		uint32_t metallicRoughnessTexture = UINT32_MAX;
		uint32_t normalTexture = UINT32_MAX;

		glm::vec4 albedoColor = glm::vec4(1.f);
		float roughness;
		float metallic;
	};

	struct glTFSceneData
	{
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
		std::vector<Texture> textures;
		std::vector<Image> images;
		std::vector<Sampler> samplers;
	};

	bool Load(const std::filesystem::path& sceneFilepath,glTFSceneData* data);
}
