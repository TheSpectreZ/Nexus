#include "nxpch.h"
#include "Material.h"
#include "Command.h"

Nexus::Ref<Nexus::Material> Nexus::Material::Create(const std::string& filepath)
{
	Ref<Material> instance = CreateRef<Material>();
	MaterialSpecifications specs = Deserialize(filepath);

	specs.AlbedoPath = "Resources/Assets/Textures/TestImage.jpg";

	instance->m_Alebdo = AssetManager::LoadFromFile<TextureAsset>(specs.AlbedoPath);

	Command::TransferMaterial(instance);

	return instance;
}

Nexus::Ref<Nexus::Texture> Nexus::Material::GetAlebdoTexture()
{
	return AssetManager::Get<TextureAsset>(m_Alebdo).Texture;
}

Nexus::MaterialSpecifications Nexus::Material::Deserialize(const std::string& path)
{
	return MaterialSpecifications();
}
