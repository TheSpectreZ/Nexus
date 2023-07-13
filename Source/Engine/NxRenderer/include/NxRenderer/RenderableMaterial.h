#pragma once
#include "NxGraphics/Meshing.h"
#include "NxGraphics/Texture.h"

namespace Nexus
{
	enum class TextureType : uint8_t
	{
		Albedo = 0,MetallicRoughness = 1,SpecularGlossiness = 2,Normal = 3,Occulsion = 4,Emissive = 5,
	};

	struct MaterialParameters
	{
		struct ParamFactors
		{
			glm::vec4 albedo;
			glm::vec3 emissive; float n1;
			glm::vec3 specular; float n2;
			float metalness, roughness, glossiness, useNormal;
		} _factors;

		std::unordered_map<TextureType, Ref<Texture>> _Maps;
	};

	class RenderableMaterial
	{
	public:
		RenderableMaterial(const Meshing::Material& params);
		~RenderableMaterial() = default;

		MaterialParameters GetParams() { return m_Params; }
	private:
		MaterialParameters m_Params;
	};

}