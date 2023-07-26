#pragma once
#include "NxCore/Object.h"
#include "NxGraphics/Meshing.h"
#include "NxGraphics/Texture.h"

#ifdef NEXUS_RENDERER_SHARED_BUILD
#define NEXUS_RENDERER_API __declspec(dllexport)
#else
#define NEXUS_RENDERER_API __declspec(dllimport)
#endif // NEXUS_RENDERER_SHARED_BUILD

namespace Nexus
{
	enum class TextureMapType : uint8_t
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

		std::unordered_map<TextureMapType, uint32_t> _Samplers;
		std::unordered_map<TextureMapType, Ref<Texture>> _Maps;
	};

	class NEXUS_RENDERER_API RenderableMaterial : public BaseAsset
	{
	public:
		RenderableMaterial(const MaterialParameters& params);
		~RenderableMaterial() = default;

		MaterialParameters GetParams() { return m_Params; }

		std::string GetAssetTypeString() override { return "MaterialAsset"; }
		uint8_t GetAssetTypeIndex() override { return 2; }
	private:
		MaterialParameters m_Params;
	};

}