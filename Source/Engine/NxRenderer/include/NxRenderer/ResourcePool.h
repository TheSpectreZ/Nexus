#pragma once
#include "NxCore/UUID.h"	
#include "NxGraphics/AssetSpecifications.h"
#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

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
		
		RESOURCE_METHOD_DECL(Texture,TextureSpecification,m_Textures)
		RESOURCE_METHOD_DECL(RenderableMesh, std::vector<Meshing::Mesh>, m_RenderableMeshes)
	private:
		std::unordered_map<UUID, Ref<Buffer>> m_UniformBuffers;
		std::unordered_map<UUID, Ref<Texture>> m_Textures;
		std::unordered_map<UUID, Ref<RenderableMesh>> m_RenderableMeshes;
	};
}