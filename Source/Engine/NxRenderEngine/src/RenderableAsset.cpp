#include "NxRenderEngine/RenderableAsset.h"
#include "NxCore/Registry.h"

std::unordered_map<Nexus::UUID, Nexus::Ref<Nexus::RenderableMesh> > Nexus::RenderableMesh::s_Pool;

Nexus::RenderableMesh::RenderableMesh(const RenderableMeshSpecification& specs)
	:Asset(specs.path,AssetType::Mesh)
{
	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	m_Meshes.resize(specs.source.meshes.size());
	for (auto i = 0; i < m_Meshes.size(); i++)
	{
		bSpecs.type = BufferType::Vertex;
		bSpecs.size = (uint32_t)(sizeof(Meshing::Vertex) * specs.source.meshes[i].vertices.size());
		bSpecs.data = (void*)specs.source.meshes[i].vertices.data();
		m_Meshes[i].m_Vb = GraphicsInterface::CreateBuffer(bSpecs);

		bSpecs.type = BufferType::Index;
		bSpecs.size = (uint32_t)(sizeof(uint32_t) * specs.source.meshes[i].indices.size());
		bSpecs.data = (void*)specs.source.meshes[i].indices.data();
		m_Meshes[i].m_Ib = GraphicsInterface::CreateBuffer(bSpecs);

		m_Meshes[i].m_Submeshes = specs.source.meshes[i].submeshes;
	}

	m_Skeleton = specs.skeleton;
}

std::unordered_map<Nexus::UUID, Nexus::Ref<Nexus::RenderableTexture> > Nexus::RenderableTexture::s_Pool;

Nexus::RenderableTexture::RenderableTexture(const RenderableTextureSpecification& specs)
	:Asset(specs.path,AssetType::Texture)
{
	m_SamplerId = specs.texture.samplerHash;

	auto& Image = specs.texture.image;

	TextureSpecification tSpecs{};
	tSpecs.extent = Extent(Image.width, Image.height);
	tSpecs.format = TextureFormat::RGBA8_SRGB;
	tSpecs.type = TextureType::TwoDim;
	tSpecs.usage = TextureUsage::ShaderSampled;
	tSpecs.pixels = (void*)Image.pixels.data();
	
	m_Texture = GraphicsInterface::CreateTexture(tSpecs);
}

std::unordered_map<Nexus::UUID, Nexus::Ref<Nexus::RenderableMaterial> > Nexus::RenderableMaterial::s_Pool;

#define LOAD_NON_EXISTING_TEX(id) if (!RenderableTexture::ContainsInPool(id))\
{\
	AssetFilePath path = AssetRegistry::Get()->LookUp(id);\
	if (!path.empty())\
	{\
		Meshing::Texture tex;\
		if (tex.Deserialize(path))\
		{\
			RenderableTextureSpecification specs{};\
			specs.texture = tex;\
			specs.path = path;\
			RenderableTexture::AddToPool(id, specs);\
		}\
	}\
}

#define LOAD_TEX_IN_MAT(id,tex,flag) if (id != UINT64_MAX)\
{\
	LOAD_NON_EXISTING_TEX(id)\
	tex = RenderableTexture::GetFromPool(id);\
	flag = 1;\
}\

Nexus::RenderableMaterial::RenderableMaterial(const RenderableMaterialSpecification& specs)
	:Asset(specs.path, AssetType::Material)
{
	// Buffer
	{
		auto& mat = specs.material;

		m_Buffer.baseColor = mat.metalicRoughness.albedoColor * mat.specularGlossiness.albedoColor;
		m_Buffer.metalness = mat.metalicRoughness.metallic;
		m_Buffer.roughness = mat.metalicRoughness.roughness;
		m_Buffer.glossiness = mat.specularGlossiness.glossiness;
		m_Buffer.specular = mat.specularGlossiness.specular;
		m_Buffer.emissive = mat.emissiveColor;
		m_Buffer.pbrType = mat.specularGlossiness.support ? 2 : 1;
	}

	// Maps
	{
		auto& mat = specs.material;
		
		uint64_t baseMap = mat.specularGlossiness.support ? mat.specularGlossiness.albedoTexture : mat.metalicRoughness.albedoTexture;
		LOAD_TEX_IN_MAT(baseMap,m_Maps.base,m_Buffer.useBaseColorMap)

		uint64_t surfaceMap = mat.specularGlossiness.support ? mat.specularGlossiness.specularGlossinessTexture : mat.metalicRoughness.metallicRoughnessTexture;
		LOAD_TEX_IN_MAT(surfaceMap, m_Maps.surface, m_Buffer.useSurfaceMap)

		LOAD_TEX_IN_MAT(mat.normalTexture, m_Maps.normal, m_Buffer.useNormalMap)
		LOAD_TEX_IN_MAT(mat.occulsionTexture, m_Maps.occulsion, m_Buffer.useOcculsionMap)
	}
}