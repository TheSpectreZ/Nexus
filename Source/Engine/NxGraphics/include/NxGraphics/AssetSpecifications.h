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

	struct NEXUS_GRAPHICS_API MeshElement
	{
		std::vector<MeshVertex> Vertices;
		std::vector<uint32_t> Indices;
	};

	struct NEXUS_GRAPHICS_API MeshSpecification
	{
		std::vector<MeshElement> elements;
		std::vector<uint64_t> materialIndices;
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
}