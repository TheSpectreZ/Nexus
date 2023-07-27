#include "NxAsset/Manager.h"
#include "NxAsset/Asset.h"

Nexus::Module::AssetManager* Nexus::Module::AssetManager::s_Instance = nullptr;

void Nexus::Module::AssetManager::Initialize()
{
	s_Instance = new AssetManager;
	s_Instance->m_RenderEnginePool = CreateRef<ResourcePool>();

	// Mesh
	{
		Meshing::Mesh mesh;
		auto [res, Id] = Importer::LoadMesh("Resources/Meshes/Cube.NxMesh", mesh, nullptr);

		s_Instance->m_RenderEnginePool->AllocateAsset<RenderableMesh>(DEFAULT_MESH_RESOURCE, mesh);
	}

	// Material
	{
		MaterialParameters params;
		params._factors.albedo = { 1.f,1.f,1.f,1.f };
		params._factors.roughness = 0.5f;
		params._factors.metalness = 0.5f;

		s_Instance->m_RenderEnginePool->AllocateAsset<RenderableMaterial>(DEFAULT_MATERIAL_RESOURCE, params);
	}

	// Texture
	{
		Meshing::Image defaultImage;
		auto [res1, id] = Importer::Loadimage("Resources/Textures/DefaultWhite.NxTex", defaultImage);

		TextureSpecification specs{};
		specs.extent = { defaultImage.width,defaultImage.height };
		specs.pixels = defaultImage.pixels.data();
		specs.format = TextureFormat::RGBA8_SRGB;
		specs.type = TextureType::TwoDim;
		specs.usage = TextureUsage::ShaderSampled;

		s_Instance->m_RenderEnginePool->AllocateAsset<Texture>(DEFAULT_TEXTURE_RESOURCE, specs);
	}
}

void Nexus::Module::AssetManager::Shutdown()
{
	delete s_Instance;
}