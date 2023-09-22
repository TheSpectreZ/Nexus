#pragma once
#include "NxCore/Asset.h"
#include "NxCore/UUID.h"
#include "NxRenderEngine/GraphicsInterface.h"
#include "Meshing.h"

namespace Nexus
{
	struct NEXUS_RENDER_ENGINE_API RenderableMeshSpecification
	{
		Meshing::MeshSource source;
		Meshing::Skeleton skeleton;
		AssetFilePath path;
	};

	class NEXUS_RENDER_ENGINE_API RenderableMesh : public Asset
	{
	public: 
		RenderableMesh(const RenderableMeshSpecification& specs);
		~RenderableMesh() override = default;

		uint64_t GetCount() { return m_Meshes.size(); }
		Ref<Buffer> GetVertexBuffer(uint64_t Index) { return m_Meshes[Index].m_Vb; }
		Ref<Buffer> GetIndexBuffer(uint64_t Index) { return m_Meshes[Index].m_Ib; }
		std::vector<Meshing::Submesh> GetSubmeshes(uint64_t Index) { return m_Meshes[Index].m_Submeshes; }

		Meshing::Skeleton GetSkeleton() { return m_Skeleton; }

		static void AddToPool(UUID Id, RenderableMeshSpecification& specs) { s_Pool[Id] = CreateRef<RenderableMesh>(specs); }
		static Ref<RenderableMesh> GetFromPool(UUID Id) { return s_Pool[Id]; }
		static void EraseFromPool(UUID Id) { s_Pool.erase(Id); }
		static bool ContainsInPool(UUID Id) { return s_Pool.contains(Id); }
		static void ClearPool() { s_Pool.clear(); }
	private:

		struct Mesh
		{
			Ref<Buffer> m_Vb, m_Ib;
			std::vector<Meshing::Submesh> m_Submeshes;
		};
		std::vector<Mesh> m_Meshes;
		Meshing::Skeleton m_Skeleton;

		static std::unordered_map<UUID, Ref<RenderableMesh> > s_Pool;
	};

	struct NEXUS_RENDER_ENGINE_API RenderableTextureSpecification
	{
		Meshing::Texture texture;
		AssetFilePath path;
		TextureFormat format = TextureFormat::RGBA8_SRGB;
	};

	class NEXUS_RENDER_ENGINE_API RenderableTexture : public Asset
	{
	public:
		RenderableTexture(const RenderableTextureSpecification& specs);
		~RenderableTexture() override = default;

		Ref<Texture> GetTexture() { return m_Texture; }
		uint32_t GetSamplerHash() { return m_SamplerId; }

		static void AddToPool(UUID Id, RenderableTextureSpecification& specs) { s_Pool[Id] = CreateRef<RenderableTexture>(specs); }
		static Ref<RenderableTexture> GetFromPool(UUID Id) { return s_Pool[Id]; }
		static void EraseFromPool(UUID Id) { s_Pool.erase(Id); }
		static bool ContainsInPool(UUID Id) { return s_Pool.contains(Id); }
		static void ClearPool() { s_Pool.clear(); }
	private:
		Ref<Texture> m_Texture;
		uint32_t m_SamplerId;;

		static std::unordered_map<UUID, Ref<RenderableTexture> > s_Pool;
	};

	struct NEXUS_RENDER_ENGINE_API RenderableMaterialSpecification
	{
		Meshing::Material material;
		AssetFilePath path;
	};

	class NEXUS_RENDER_ENGINE_API RenderableMaterial : public Asset
	{
	public:
		RenderableMaterial(const RenderableMaterialSpecification& specs);
		~RenderableMaterial() override = default;

		static void AddToPool(UUID Id, RenderableMaterialSpecification& specs) { s_Pool[Id] = CreateRef<RenderableMaterial>(specs); }
		static Ref<RenderableMaterial> GetFromPool(UUID Id) { return s_Pool[Id]; }
		static void EraseFromPool(UUID Id) { s_Pool.erase(Id); }
		static bool ContainsInPool(UUID Id) { return s_Pool.contains(Id); }
		static void ClearPool() { s_Pool.clear(); }

		struct MaterialBuffer
		{
			glm::vec4 baseColor;
			glm::vec3 emissive; float n0;
			glm::vec3 specular; int pbrType;

			int useBaseColorMap = -1;
			int useSurfaceMap = -1;
			int useEmissiveMap = -1;
			int useNormalMap = -1;
			int useOcculsionMap = -1;

			float metalness, roughness, glossiness;
		};

		struct MaterialMaps
		{
			Ref<RenderableTexture> base, normal, surface, occulsion;
		};

		MaterialBuffer GetBuffer() { return m_Buffer; }
		MaterialMaps GetMaps() { return m_Maps; }

	private:
		MaterialBuffer m_Buffer;
		MaterialMaps m_Maps;

		static std::unordered_map<UUID, Ref<RenderableMaterial> > s_Pool;
	};
}
