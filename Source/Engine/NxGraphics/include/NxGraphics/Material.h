#pragma once
#include "NxCore/Base.h"
#include "Texture.h"

#include "glm/glm.hpp"

#ifdef NEXUS_GRAPHICS_SHARED_BUILD
#define NEXUS_GRAPHICS_API __declspec(dllexport)
#else
#define NEXUS_GRAPHICS_API __declspec(dllimport)
#endif // NEXUS_GRAPHICS_SHARED_BUILD


namespace Nexus
{
	class NEXUS_GRAPHICS_API Material
	{
	public:
		Material() = default;
		virtual ~Material() = default;
	};

	struct NEXUS_GRAPHICS_API PBRMaterialParams
	{
		glm::vec3 color;
		float metalness, roughness;
	};

	struct NEXUS_GRAPHICS_API PBRMaterialSpecifications
	{
		PBRMaterialParams params;
		Ref<Texture> albedo, normal, metallicRoughness;
	};

	class NEXUS_GRAPHICS_API PBRMaterial : public Material
	{
	public:
		PBRMaterial(const PBRMaterialSpecifications& specs);
		~PBRMaterial() override = default;
	private:
		Ref<Texture> m_AlbedoMap, m_NormalMap, m_MetallicRoughnessMap;
		PBRMaterialParams m_Params;
	};
}
