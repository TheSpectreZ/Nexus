#pragma once
#include "NxCore/UUID.h"	

#include "NxRenderEngine/GraphicsInterface.h"
#include "NxRenderEngine/EnvironmentBuilder.h"	
#include "NxRenderEngine/RenderableMesh.h"
#include "NxRenderEngine/RenderableMaterial.h"

namespace Nexus
{
#define DEFAULT_MESH_RESOURCE (uint64_t)0
#define DEFAULT_MATERIAL_RESOURCE (uint64_t)1
#define DEFAULT_TEXTURE_RESOURCE (uint64_t)2

	class NEXUS_RENDERER_API ResourcePool
	{
		static ResourcePool* s_Instance;
	public:
		static ResourcePool* Get() { return s_Instance; }
		
		ResourcePool();
		~ResourcePool();

		Ref<Buffer> GetUniformBuffer(UUID HashID) { return m_UniformBuffers[HashID]; }
		Ref<Buffer> AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle);
		void DeallocateUniformBuffer(UUID HashID);
		
		Ref<Sampler> GetSampler(uint32_t HashId);
		Ref<Sampler> GetSampler(const SamplerSpecification& specs);
		
		Ref<Environment> AllocateEnvironment(UUID HashID);
		void DeallocateEnvironment(UUID HashID);
	private:
		std::unordered_map<UUID, Ref<Buffer>> m_UniformBuffers;
		std::unordered_map<UUID, Ref<Environment>> m_Environments;
		std::unordered_map<uint32_t, Ref<Sampler>> m_Samplers;

		std::unordered_map<UUID, Ref<BaseAsset>> m_Assets;

		Ref<RenderableMaterial> AllocMaterialImpl(Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures);
	public:
		template<typename T,typename... Args>
		inline Ref<T> AllocateAsset(UUID Id,Args&&... args)
		{
			if (!m_Assets.contains(Id))
				m_Assets[Id] = CreateRef<T>(std::forward<Args>(args)...);

			return DynamicPointerCast<T>(m_Assets[Id]);
		}

		template<typename T>
		inline Ref<T> GetAsset(UUID Id)
		{
			return DynamicPointerCast<T>(m_Assets[Id]);
		}

		inline void DeallocateAsset(UUID Id)
		{
			if (m_Assets.contains(Id))
				m_Assets.erase(Id);
		}
	};

	template<>
	inline Ref<Texture> Nexus::ResourcePool::AllocateAsset(UUID Id, TextureSpecification& specs)
	{
		if (!m_Assets.contains(Id))
			m_Assets[Id] = GraphicsInterface::CreateTexture(specs);

		return DynamicPointerCast<Texture>(m_Assets[Id]);
	}

	template<> 
	inline Ref<RenderableMaterial> Nexus::ResourcePool::AllocateAsset(UUID Id, Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures)
	{
		if (!m_Assets.contains(Id))
			m_Assets[Id] = this->AllocMaterialImpl(specs, textures);

		return DynamicPointerCast<RenderableMaterial>(m_Assets[Id]);
	}
}