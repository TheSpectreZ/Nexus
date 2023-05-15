#pragma once
#include "Assets/Asset.h"
#include "glm/glm.hpp"

namespace Nexus
{ 
	class Material : public Asset
	{
	public:
		Material() = default;
		~Material() override = default;

		float IsTextureUsed() { return m_UseTexture; }
		void SetUseTexture(bool use) { use ? m_UseTexture = 1.f : m_UseTexture = 0.f; }

		glm::vec3 GetAlbedoColor() { return m_AlbedoColor; }
		void SetAlbedoColor(const glm::vec3& color) { m_AlbedoColor = color; }

		UUID GetAlbedo() { return m_Albedo; }
		void SetAlbedo(UUID Id) { m_Albedo = Id; }
	private:
		float m_UseTexture = 0.f;
		glm::vec3 m_AlbedoColor = glm::vec3(1.f);
		UUID m_Albedo = NullUUID;
	};
}