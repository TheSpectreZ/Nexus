#pragma once
#include <filesystem>
#include "NxGraphics/Renderables.h"

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif // NEXUS_ASSET_SHARED_BUILD


namespace Nexus::Importer::glTF
{
	struct VertexData
	{
		glm::vec3 position, normal, tangent, bitangent;
		glm::vec2 texCoord;
	};

	struct Submesh
	{
		std::vector<VertexData> vertices;
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
	
	enum class SampFilter
	{
		Nearest, Linear
	};

	enum class SampWrapMode
	{
		Repeat, Mirrored_Repeat, Clamped_To_Edge
	};

	struct Sampler
	{
		SampFilter Near, Far;
		SampWrapMode U, V, W;
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
		float roughness = 0.5f;
		float metallic = 0.5f;
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
	
	bool NEXUS_ASSET_API Load(const std::filesystem::path& filepath, MeshSpecifications* specs);
}
