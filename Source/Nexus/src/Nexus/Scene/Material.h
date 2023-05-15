#pragma once
#include "Assets/Asset.h"
#include "glm/glm.hpp"

namespace Nexus
{ 
	class Material : public Asset
	{
	public:
		static Ref<Material> Create();

		Material() = default;
		~Material() override = default;

		UUID GetAlbedo() { return m_Albedo; }
		void SetAlbedo(UUID Id) { m_Albedo = Id; }
	private:
		UUID m_Albedo = NullUUID;
	};
}