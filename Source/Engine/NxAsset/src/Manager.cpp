#include "NxAsset/Manager.h"
#include "NxAsset/Asset.h"

Nexus::Manager* Nexus::Manager::s_Instance = nullptr;

void Nexus::Manager::Initialize()
{
	s_Instance = new Manager;

	// Mesh
	{
		Meshing::Mesh mesh;
		auto [res, Id] = Importer::LoadMesh("Resources/Meshes/Cube.NxMesh", mesh, nullptr);
		s_Instance->Allocate<RenderableMesh>(DEFAULT_RESOURCE, mesh);
	}

	// Material
	{
		MaterialParameters params;
		params._factors.albedo = { 1.f,1.f,1.f,1.f };
		params._factors.roughness = 0.5f;
		params._factors.metalness = 0.5f;

		s_Instance->Allocate<RenderableMaterial>(DEFAULT_RESOURCE, params);
	}

	{
		
		Meshing::Image defaultImage;
		auto [res1, id] = Importer::Loadimage("Resources/Textures/DefaultWhite.NxTex", defaultImage);

		TextureSpecification specs{};
		specs.extent = { defaultImage.width,defaultImage.height };
		specs.pixels = defaultImage.pixels.data();
		specs.format = TextureFormat::RGBA8_SRGB;
		specs.type = TextureType::TwoDim;
		specs.usage = TextureUsage::ShaderSampled;

		s_Instance->Allocate<Texture>(DEFAULT_RESOURCE, specs);
	}
}

void Nexus::Manager::Shutdown()
{
	delete s_Instance;
}

bool Nexus::Manager::Has(UUID Id)
{
	return m_AssetPool.contains(Id);
}

void Nexus::Manager::Free(UUID Id)
{
	m_AssetPool.erase(Id);
}

