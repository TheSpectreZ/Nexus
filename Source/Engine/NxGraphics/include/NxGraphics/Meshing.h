#pragma once
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "TypeImpls.h"

namespace Nexus
{
	namespace Meshing
	{
		struct Vertex
		{
			glm::vec3 position, normal, tangent, bitangent;
			glm::vec2 texCoord0, texCoord1;
			glm::ivec4 joints; glm::vec4 weights;
		};

		struct Submesh
		{
			uint32_t indexOffset, indexSize;
			uint32_t materialIndex;
		};

		struct Mesh
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<Submesh> submeshes;
		};

		struct Sampler
		{
			SamplerFilter Near;
			SamplerFilter Far;
			SamplerWrapMode U, V, W;

			uint32_t GetHash() const
			{
				uint32_t Id = 0;
				Id += (uint32_t)Far * 1;
				Id += (uint32_t)Near * 10;
				Id += (uint32_t)U * 100;
				Id += (uint32_t)V * 1000;
				Id += (uint32_t)W * 10000;

				return Id;
			}

			void ResolveHash(uint32_t HashId)
			{
				Far = (SamplerFilter)((HashId / 1) % 10);
				Near = (SamplerFilter)((HashId / 10) % 10);
				U = (SamplerWrapMode)((HashId / 100) % 10);
				V = (SamplerWrapMode)((HashId / 1000) % 10);
				W = (SamplerWrapMode)((HashId / 10000) % 10);
			};
		};

		struct Image
		{
			std::string fileName;

			std::vector<uint8_t> pixels;
			uint32_t width, height, channels;
		};

		struct Texture
		{
			Image image;
			uint32_t samplerHash;
		};

		struct Material
		{
			std::string Name;
			struct TexCoords
			{
				uint8_t albedo;
				uint8_t normal;
				uint8_t occlusion;
				uint8_t emissive;

				uint8_t metallicRoughness;
				uint8_t specularGlossiness;
			} textureCoords;

			struct pbrMetallicRoughness
			{
				glm::vec4 albedoColor{1.f};
				float roughness{ 1.f };
				float metallic{ 1.f };

				uint64_t albedoTexture = UINT64_MAX;
				uint64_t metallicRoughnessTexture = UINT64_MAX;
			} metalicRoughness;

			struct pbrSpecularGlossiness
			{
				bool support = false;
				glm::vec4 aldeboColor{1.f};
				glm::vec3 specular;
				float glossiness;

				uint64_t albedoTexture = UINT64_MAX;
				uint64_t specularGlossinessTexture = UINT64_MAX;
			} specularGlossiness;

			uint64_t normalTexture = UINT64_MAX;
			uint64_t occulsionTexture = UINT64_MAX;

			uint64_t emissiveTexture = UINT64_MAX;
			glm::vec3 emissiveColor{0.f};
		};

		struct Node
		{
			std::string name;

			Node* parent;
			std::vector<Node*> children;

			uint32_t Index;
			uint32_t mesh;
			int32_t skin = -1;

			glm::vec3 translation{0.f};
			glm::vec3 scale{1.f};
			glm::quat rotation{};

			glm::mat4 matrix{1.f};
		};

		struct Skin
		{
			std::string Name;
			Node* skeletonRoot = nullptr;
			std::vector<Node*> Joints;
			std::vector<glm::mat4> InverseBindMatrices;
		};

		struct Animation
		{
			struct Sampler
			{
				enum InterpolationType { Linear, Step, CubicSpline };
				InterpolationType type;

				std::vector<float> input;
				std::vector<glm::vec4> output;
			};

			struct Channel
			{
				enum PathType { Translation, Rotation, Scale };
				PathType type;

				Node* node;
				uint32_t samplerIndex;
			};

			std::string Name;
			std::vector<Sampler> samplers;
			std::vector<Channel> channels;

			float start = FLT_MAX;
			float end = FLT_MIN;
		};

		struct Scene
		{
			// Static Mesh
			Mesh mesh;

			// Skeleton
			std::vector<Node> nodes;
			std::vector<Skin> skins;

			// Materials
			std::vector<Material> materials;
			std::vector<Texture> textures;

			// Animations
			std::vector<Animation> animations;
		};
	}
}