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
			glm::vec3 emissive; float n0;
			glm::vec3 specular;
			
			int pbrType;
			int useBaseColorMap = -1;
			int useSurfaceMap = -1;
			int useEmissiveMap = -1;
			int useNormalMap = -1;
			int useOculsionMap = -1;

			float metalness, roughness, glossiness;
		} _factors;

		std::unordered_map<TextureType, uint32_t> _Samplers;
		std::unordered_map<TextureType, Ref<Texture>> _Maps;
	};

	class RenderableMaterial
	{
	public:
		RenderableMaterial(const Meshing::Material& params, const std::unordered_map<TextureType, uint32_t>& Samplers);
		~RenderableMaterial() = default;

		MaterialParameters GetParams() { return m_Params; }
	private:
		MaterialParameters m_Params;
	};

}