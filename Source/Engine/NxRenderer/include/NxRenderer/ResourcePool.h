#pragma once
#include "NxCore/UUID.h"	
#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

namespace Nexus
{

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
		
		Ref<RenderableMesh> GetRenderableMesh(UUID HashID) { return m_RenderableMeshes[HashID]; }
		Ref<RenderableMesh> AllocateRenderableMesh(const MeshSpecification& specs,UUID HashID);
		void DeallocateRenderableMesh(UUID HashID);

		Ref<Texture> GetTexture(UUID HashID) { return m_Textures[HashID]; }
		Ref<Texture> AllocateTexture(const TextureSpecification& specs, UUID HashID);
		void DeallocateTexture(UUID HashID);
	private:
		std::unordered_map<UUID, Ref<Buffer>> m_UniformBuffers;
		std::unordered_map<UUID, Ref<RenderableMesh>> m_RenderableMeshes;
		std::unordered_map<UUID, Ref<Texture>> m_Textures;
	};
}