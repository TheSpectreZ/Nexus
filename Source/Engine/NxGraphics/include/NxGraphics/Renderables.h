#pragma once
#include "NxCore/Base.h"
#include "AssetSpecifications.h"
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

	class NEXUS_GRAPHICS_API Mesh : public Renderable
	{
	public:
		Mesh() :Renderable(RenderableType::Mesh) {}
		virtual ~Mesh() override = default;

		const std::vector<SubmeshElement>& GetSubmeshes() { return m_Submeshs; }
	protected:
		std::vector<SubmeshElement> m_Submeshs;
	};
}
