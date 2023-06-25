#include "NxAsset/Asset.h"

void Nexus::MeshAsset::Make(const std::string& sourceFilepath)
{
	Importer::glTF::glTFSceneData data;
	Importer::glTF::Load(sourceFilepath, &data);

	m_Meshes = data.meshes;
}
