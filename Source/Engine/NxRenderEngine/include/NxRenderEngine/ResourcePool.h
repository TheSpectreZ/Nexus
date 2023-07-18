#pragma once
#include "NxCore/UUID.h"	
#include "NxRenderEngine/RenderableMesh.h"
#include "NxRenderEngine/RenderableMaterial.h"
#include "NxRenderEngine/GraphicsInterface.h"
#include "NxRenderEngine/EnvironmentBuilder.h"	

namespace Nexus
{
#define RESOURCE_METHOD_DECL(Resource,Specs,m_Member)\
	Ref<Resource> Get##Resource(UUID HashId) {return m_Member[HashId];}\
	Ref<Resource> Allocate##Resource(const Specs& specs,UUID HashID);\
	void Deallocate##Resource(UUID HashID);

	class NEXUS_RENDERER_API ResourcePool
	{
		static ResourcePool* s_Instance;
	public:
		static ResourcePool* Get() { return s_Instance; }
		static void Initialize();
		static void Shutdown();

		Ref<Buffer> GetUniformBuffer(UUID HashID) { return m_UniformBuffers[HashID]; }
		Ref<Buffer> AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle);
		void DeallocateUniformBuffer(UUID HashID);
		
		Ref<Sampler> GetSampler(uint32_t HashId);
		Ref<Sampler> GetSampler(const SamplerSpecification& specs);

		RESOURCE_METHOD_DECL(Texture,TextureSpecification,m_Textures)
		RESOURCE_METHOD_DECL(RenderableMesh, Meshing::Mesh, m_RenderableMeshes)
		
		Ref<RenderableMaterial> GetRenderableMaterial(UUID HashId) { return m_RenderableMaterials[HashId]; }
		Ref<RenderableMaterial> AllocateRenderableMaterial(const Meshing::Material& specs, std::unordered_map<uint8_t, Meshing::Texture>& textures, UUID HashID);
		void DeallocateRenderableMaterial(UUID HashID);;

		Ref<Environment> AllocateEnvironment(UUID HashID);
		void DeallocateEnvironment(UUID HashID);
	private:
		std::unordered_map<UUID, Ref<Buffer>> m_UniformBuffers;
		std::unordered_map<UUID, Ref<Texture>> m_Textures;
		std::unordered_map<UUID, Ref<RenderableMesh>> m_RenderableMeshes;
		std::unordered_map<UUID, Ref<RenderableMaterial>> m_RenderableMaterials;
		
		std::unordered_map<UUID, Ref<Environment>> m_Environments;
		
		std::unordered_map<uint32_t, Ref<Sampler>> m_Samplers;
	};
}