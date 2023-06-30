#pragma once
#include "NxCore/UUID.h"	
#include "NxRenderer/GraphicsInterface.h"

namespace Nexus
{
	class ResourcePool
	{
	public:
		ResourcePool() = default;
		~ResourcePool() = default;

		void Initialize();
		void Shutdown();

		Ref<Buffer> GetUniformBuffer(UUID HashID) { return m_UniformBuffers[HashID]; }
		void AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle);
		void DeallocateUniformBuffer(UUID HashID);
		void UpdateUniformBuffer(UUID HashID, void* Data);
	private:
		std::unordered_map<UUID, Ref<Buffer>> m_UniformBuffers;
	};
}