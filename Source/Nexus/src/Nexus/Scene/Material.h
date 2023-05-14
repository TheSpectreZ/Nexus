#pragma once
#include "Assets/Asset.h"

namespace Nexus
{ 
	class Material : public Asset
	{
	public:
		Material() = default;
		~Material() override = default;

		UUID GetAlbedo() { return m_Albedo; }
		void SetAlbedo(UUID Id) { m_Albedo = Id; }
	private:
		UUID m_Albedo = NullUUID;
	};
}