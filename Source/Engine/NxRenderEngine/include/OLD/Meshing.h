#pragma once
#include "NxCore/Asset.h"	

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "NxGraphics/TypeImpls.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define	NEXUS_RENDER_ENGINE_API __declspec(dllexport)
#else
#define NEXUS_RENDER_ENGINE_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

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

		class NEXUS_RENDER_ENGINE_API MeshSource : public BinarySerializer
		{
		public:
			std::vector<Mesh> meshes;
		protected:
			BinaryBlob OnSerialize() override;
			void OnDeserialize(BinaryBlob blob) override;
		};

		struct NEXUS_RENDER_ENGINE_API Sampler
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
		};

		struct Image
		{
			std::string fileName;

			std::vector<uint8_t> pixels;
			uint32_t width, height, channels;
		};

		class NEXUS_RENDER_ENGINE_API Texture : public BinarySerializer
		{
		public:
			Image image;
			uint32_t samplerHash = UINT32_MAX;
		protected:
			BinaryBlob OnSerialize() override;
			void OnDeserialize(BinaryBlob blob) override;
		};

		class NEXUS_RENDER_ENGINE_API Material : public JsonSerializer
		{
		public:
			std::string Name;
			struct TexCoords
			{
				uint8_t baseColor = 0;
				uint8_t normal = 0;
				uint8_t occlusion = 0;
				uint8_t emissive = 0;

				uint8_t metallicRoughness = 0;
				uint8_t specularGlossiness = 0;
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
				glm::vec4 albedoColor{1.f};
				glm::vec3 specular;
				float glossiness;

				uint64_t albedoTexture = UINT64_MAX;
				uint64_t specularGlossinessTexture = UINT64_MAX;
			} specularGlossiness;

			uint64_t normalTexture = UINT64_MAX;
			uint64_t occulsionTexture = UINT64_MAX;

			uint64_t emissiveTexture = UINT64_MAX;
			glm::vec3 emissiveColor{0.f};
		protected:
			nlohmann::json OnSerialize() override;
			void OnDeserialize(const nlohmann::json& blob) override;
		};

		struct Node
		{
			std::string name;

			int32_t parent = -1;
			std::vector<int32_t> children;

			uint32_t Index;
			uint32_t mesh = UINT32_MAX;
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

		class NEXUS_RENDER_ENGINE_API Skeleton : public BinarySerializer
		{
		public:
			std::vector<Node> nodes;
			std::vector<Skin> skins;
		protected:
			BinaryBlob OnSerialize() override;
			void OnDeserialize(BinaryBlob blob) override;
		};

		class NEXUS_RENDER_ENGINE_API Animation : public BinarySerializer
		{
		public:
			struct Sampler
			{
				enum InterpolationType : uint8_t { Linear, Step, CubicSpline };
				InterpolationType type;

				std::vector<float> input;
				std::vector<glm::vec4> output;
			};

			struct Channel
			{
				enum PathType : uint8_t { Translation, Rotation, Scale };
				PathType type;

				uint32_t node;
				uint32_t samplerIndex;
			};

			std::string Name;
			std::vector<Sampler> samplers;
			std::vector<Channel> channels;

			float start = FLT_MAX;
			float end = FLT_MIN;
		protected:
			BinaryBlob OnSerialize() override;
			void OnDeserialize(BinaryBlob blob) override;
		};

		struct Scene
		{
			// Mesh
			MeshSource meshSource;
			
			// Skeleton
			Skeleton skeleton;

			// Materials
			std::vector<Material> materials;
			std::vector<Texture> textures;

			// Animations
			std::vector<Animation> animations;
		};
	}
}