#pragma once
#include "NxCore/Base.h"
#include "NxCore/UUID.h"
#include "NxCore/Object.h"

#include "NxGraphics/Texture.h"
#include "NxRenderEngine/GraphicsInterface.h"
#include "NxRenderEngine/RenderableMesh.h"
#include "NxRenderEngine/RenderableMaterial.h"	

#include <unordered_map>

#ifdef NEXUS_ASSET_SHARED_BUILD
#define NEXUS_ASSET_API __declspec(dllexport)
#else
#define NEXUS_ASSET_API __declspec(dllimport)
#endif

namespace Nexus
{
#define DEFAULT_RESOURCE (uint64_t)0

	class NEXUS_ASSET_API Manager
	{
		enum : uint8_t
		{
			assetType_None = 0,
			assetType_Mesh = 1,
			assetType_Material = 2,
			assetType_Texture = 3,
		};

		static Manager* s_Instance;
	public:
		static Manager* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		template<typename T,typename... Args>
		Ref<T> Allocate(UUID Id, Args&&... args);

		bool Has(UUID Id);

		template<typename T>
		Ref<T> Get(UUID Id);

		template<typename T>
		Ref<T> Get(uint64_t Id) { return Get<T>(UUID(Id)); }

		// For Sampler only
		template<typename T,typename... Args>
		Ref<T> Get(Args&&... args);

		void Free(UUID Id);
	private:
		std::unordered_map< UUID, Ref<BaseAsset> > m_AssetPool;
		std::unordered_map< uint32_t, Ref<Sampler> > m_SamplerPool;
	};

	template<typename T, typename ...Args>
	inline Ref<T> Manager::Allocate(UUID Id, Args&& ...args)
	{
		if (!m_AssetPool.contains(Id))
			m_AssetPool[Id] = CreateRef<T>(args...);

		return Nexus::DynamicPointerCast<T>(m_AssetPool[Id]);
	}

	template<typename T>
	inline Ref<T> Manager::Get(UUID Id)
	{
		return Nexus::DynamicPointerCast<T>(m_AssetPool[Id]);
	}

	template<>
	inline Ref<Texture> Manager::Allocate(UUID Id, TextureSpecification& specs)
	{
		if (!m_AssetPool.contains(Id))
			m_AssetPool[Id] = GraphicsInterface::CreateTexture(specs);

		return DynamicPointerCast<Texture>(m_AssetPool[Id]);
	}

	template<>
	inline Nexus::Ref<Nexus::RenderableMaterial> Nexus::Manager::Allocate(UUID Id, Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures)
	{
		if (!m_AssetPool.contains(Id))
		{
			std::unordered_map<TextureMapType, uint32_t> Samplers;

			TextureSpecification ts{};
			ts.format = TextureFormat::RGBA8_SRGB;
			ts.type = TextureType::TwoDim;
			ts.usage = TextureUsage::ShaderSampled;

			if (textures.contains((uint8_t)TextureMapType::Albedo))
			{
				Samplers[TextureMapType::Albedo] = textures[(uint32_t)TextureMapType::Albedo].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::Albedo].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				if (specs.specularGlossiness.support)
					s_Instance->Allocate<Texture>(specs.specularGlossiness.albedoTexture, ts);
				else
					s_Instance->Allocate<Texture>(specs.metalicRoughness.albedoTexture, ts);
			}

			if (textures.contains((uint8_t)TextureMapType::Emissive))
			{
				Samplers[TextureMapType::Emissive] = textures[(uint32_t)TextureMapType::Emissive].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::Emissive].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				s_Instance->Allocate<Texture>(specs.emissiveTexture, ts);
			}

			if (textures.contains((uint8_t)TextureMapType::Normal))
			{
				Samplers[TextureMapType::Normal] = textures[(uint32_t)TextureMapType::Normal].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::Normal].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				s_Instance->Allocate<Texture>(specs.normalTexture, ts);
			}

			if (textures.contains((uint8_t)TextureMapType::Occulsion))
			{
				Samplers[TextureMapType::Occulsion] = textures[(uint32_t)TextureMapType::Occulsion].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::Occulsion].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				s_Instance->Allocate<Texture>(specs.occulsionTexture, ts);
			}

			if (textures.contains((uint8_t)TextureMapType::SpecularGlossiness))
			{
				Samplers[TextureMapType::SpecularGlossiness] = textures[(uint32_t)TextureMapType::SpecularGlossiness].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::SpecularGlossiness].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				s_Instance->Allocate<Texture>(specs.specularGlossiness.specularGlossinessTexture, ts);
			}

			if (textures.contains((uint8_t)TextureMapType::MetallicRoughness))
			{
				Samplers[TextureMapType::MetallicRoughness] = textures[(uint32_t)TextureMapType::MetallicRoughness].samplerHash;

				auto& albedo = textures[(uint8_t)TextureMapType::MetallicRoughness].image;
				ts.extent = { albedo.width,albedo.height };
				ts.pixels = albedo.pixels.data();

				s_Instance->Allocate<Texture>(specs.metalicRoughness.metallicRoughnessTexture, ts);
			}

			MaterialParameters mParams;
			{
				mParams._Samplers = Samplers;

				mParams._factors.albedo = specs.specularGlossiness.support ? specs.specularGlossiness.aldeboColor : specs.metalicRoughness.albedoColor;
				mParams._factors.glossiness = specs.specularGlossiness.glossiness;
				mParams._factors.metalness = specs.metalicRoughness.metallic;
				mParams._factors.roughness = specs.metalicRoughness.roughness;
				mParams._factors.specular = specs.specularGlossiness.specular;
				mParams._factors.emissive = specs.emissiveColor;

				if (specs.specularGlossiness.support)
				{
					mParams._factors.pbrType = 2;
					if (specs.specularGlossiness.albedoTexture != UINT64_MAX)
					{
						mParams._Maps[TextureMapType::Albedo] = Get<Texture>(specs.specularGlossiness.albedoTexture);
						mParams._factors.useBaseColorMap = 1;
					}
				}
				else
				{
					mParams._factors.pbrType = 1;
					if (specs.metalicRoughness.albedoTexture != UINT64_MAX)
					{
						mParams._Maps[TextureMapType::Albedo] = Get<Texture>(specs.metalicRoughness.albedoTexture);
						mParams._factors.useBaseColorMap = 1;
					}
				}

				if (specs.normalTexture != UINT64_MAX)
				{
					mParams._Maps[TextureMapType::Normal] = Get<Texture>(specs.normalTexture);
					mParams._factors.useNormalMap = 1;
				}

				if (specs.occulsionTexture != UINT64_MAX)
				{
					mParams._Maps[TextureMapType::Occulsion] = Get<Texture>(specs.occulsionTexture);
					mParams._factors.useOculsionMap = 1;
				}

				if (specs.emissiveTexture != UINT64_MAX)
				{
					mParams._Maps[TextureMapType::Emissive] = Get<Texture>(specs.emissiveTexture);
					mParams._factors.useEmissiveMap = 1;
				}
				if (specs.metalicRoughness.metallicRoughnessTexture != UINT64_MAX)
				{
					mParams._Maps[TextureMapType::MetallicRoughness] = Get<Texture>(specs.metalicRoughness.metallicRoughnessTexture);
					mParams._factors.useSurfaceMap = 1;
				}
				if (specs.normalTexture != UINT64_MAX)
				{
					mParams._Maps[TextureMapType::SpecularGlossiness] = Get<Texture>(specs.specularGlossiness.specularGlossinessTexture);
					mParams._factors.useSurfaceMap = 1;
				}
			}

			m_AssetPool[Id] = CreateRef<RenderableMaterial>(mParams);
		}

		return DynamicPointerCast<RenderableMaterial>(m_AssetPool[Id]);
	}

	template<>
	inline Nexus::Ref<Nexus::Sampler> Nexus::Manager::Get(const SamplerSpecification& specs)
	{
		uint32_t hash = specs.sampler.GetHash();
		if (!m_SamplerPool[hash])
			m_SamplerPool[hash] = GraphicsInterface::CreateSampler(specs);

		return m_SamplerPool[hash];
	}

	template<>
	inline Nexus::Ref<Nexus::Sampler> Nexus::Manager::Get(uint32_t& samplerHashCode)
	{
		if (!m_SamplerPool[samplerHashCode])
		{
			SamplerSpecification specs{};
			specs.sampler.ResolveHash(samplerHashCode);

			m_SamplerPool[samplerHashCode] = GraphicsInterface::CreateSampler(specs);
		}

		return m_SamplerPool[samplerHashCode];
	}
}
