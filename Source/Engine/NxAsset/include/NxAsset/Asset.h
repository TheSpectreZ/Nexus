#pragma once
#include <string>
#include "NxAsset/glTFImporter.h"
#include "NxCore/UUID.h"

#include "glm/glm.hpp"	

namespace Nexus
{
	enum class AssetType
	{
		Mesh, Texture
	};

	class Asset
	{
		friend class AssetManager;
	public:
		Asset() = default;
		virtual ~Asset() = default;

		AssetType GetType() { return m_Type; }
		std::string GetPath() { return m_Path; }
	protected:
		AssetType m_Type;
		std::string m_Path;
	};

	class MeshAsset : public Asset
	{
	public:
		MeshAsset() = default;
		~MeshAsset() override = default;

		void Make(const std::string& sourceFilepath);
	private:
		std::vector<Importer::glTF::Mesh> m_Meshes;
	};
}