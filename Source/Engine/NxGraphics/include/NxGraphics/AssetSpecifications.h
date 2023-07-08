#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "TypeImpls.h"

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif

namespace Nexus
{
	struct NEXUS_GRAPHICS_API MeshVertex
	{
		glm::vec3 position, normal, tangent, bitangent;
		glm::vec2 texCoord;
		glm::vec4 color;
	};
	
	struct NEXUS_GRAPHICS_API SubmeshElement
	{
		uint32_t VertexOff, IndexOff;
		uint32_t VertexSize, IndexSize;
		uint32_t materialIndex;
	};

	struct NEXUS_GRAPHICS_API MeshElement
	{
		std::vector<MeshVertex> Vertices;
		std::vector<uint32_t> Indices;
	};
	struct NEXUS_GRAPHICS_API TextureSpecification
	{
		Extent extent;
		void* pixeldata;

		~TextureSpecification()
		{
			extent = { 0,0 };
			delete[] pixeldata;
		}
	};

	struct NEXUS_GRAPHICS_API SamplerSpecification
	{
		SamplerFilter Near;
		SamplerFilter Far;
		SamplerWrapMode U, V, W;
	};

	enum class NEXUS_GRAPHICS_API TextureMapType
	{
		Albedo = 0, Normal = 1, MetallicRoughness = 2
	};

	struct NEXUS_GRAPHICS_API TextureMapElement
	{
		uint32_t textureId;
		uint32_t samplerId;
	};

	struct NEXUS_GRAPHICS_API MaterialElement
	{
		std::unordered_map<TextureMapType, uint32_t> Maps;
		
		glm::vec4 albedo;
		float metalness, roughness;
	};

	struct NEXUS_GRAPHICS_API MaterialSpecification
	{
		std::vector<TextureSpecification> textures;
		std::vector<SamplerSpecification> samplers;
		std::vector<TextureMapElement> texElements;
		std::vector<MaterialElement> matElements;
	};

	struct NEXUS_GRAPHICS_API MeshSpecification
	{
		MeshElement mesh;
		std::vector<SubmeshElement> submeshes;
		MaterialSpecification materials;
	};
}