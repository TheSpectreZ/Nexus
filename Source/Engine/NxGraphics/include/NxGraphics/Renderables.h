#pragma once
#include <vector>
#include "glm/glm.hpp"

#include "NxCore/Base.h"
#include "Buffer.h"

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif

namespace Nexus
{
	enum class NEXUS_GRAPHICS_API RenderableType
	{
		Mesh
	};

	class NEXUS_GRAPHICS_API Renderable
	{
	public:
		Renderable(RenderableType type) :m_Type(type) {}
		virtual ~Renderable() = default;

		RenderableType GetType() { return m_Type; }
	protected:
		RenderableType m_Type;

		Ref<Buffer> m_VertexBuffer;
		Ref<Buffer> m_IndexBuffer;
	};

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

	struct NEXUS_GRAPHICS_API MeshSpecifications
	{
		std::vector<MeshElement> elements;
		std::vector<uint64_t> materialIndices;
	};

	class NEXUS_GRAPHICS_API Mesh : public Renderable
	{
	public:
		Mesh() :Renderable(RenderableType::Mesh) {}
		~Mesh() override = default;
	};
}
