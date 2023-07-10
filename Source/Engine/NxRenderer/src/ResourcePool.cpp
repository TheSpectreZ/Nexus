#include "NxRenderer/ResourcePool.h"

#include "NxCore/Assertion.h"

Nexus::ResourcePool* Nexus::ResourcePool::s_Instance = nullptr;

void Nexus::ResourcePool::Initialize()
{
	s_Instance = new ResourcePool;
}

void Nexus::ResourcePool::Shutdown()
{
	s_Instance->m_Textures.clear();
	s_Instance->m_MaterialTables.clear();
	s_Instance->m_RenderableMeshes.clear();
	s_Instance->m_UniformBuffers.clear();
	delete s_Instance;
}

Nexus::Ref<Nexus::Buffer> Nexus::ResourcePool::AllocateUniformBuffer(Ref<Shader> shader, UniformBufferHandle handle)
{
	if (!m_UniformBuffers.contains(handle.hashId))
	{
		ShaderResouceHeapLayoutBinding* binding = nullptr;
		shader->GetShaderResourceHeapLayoutBinding(binding, handle.set, handle.binding);

		NEXUS_BREAK_ASSERT((binding == nullptr), "Shader Set-Binding Doesn't Exists: %i-%i", handle.set, handle.binding);

		BufferSpecification specs{};
		specs.type = BufferType::Uniform;
		specs.size = binding->bufferSize;
		specs.data = nullptr;
		specs.cpuMemory = true;

		m_UniformBuffers[handle.hashId] = GraphicsInterface::CreateBuffer(specs);
	}

	return m_UniformBuffers[handle.hashId];
}

void Nexus::ResourcePool::DeallocateUniformBuffer(UUID HashID)
{
	if (m_UniformBuffers.contains(HashID))
		m_UniformBuffers.erase(HashID);
}

#define RESOURCE_ALLOC_METHOD_GI_IMP(Resource,Specs,m_Member)\
Nexus::Ref<Nexus::##Resource> Nexus::ResourcePool::Allocate##Resource(const Specs& specs,UUID HashId)\
{\
	if(!m_Member.contains(HashId))\
		m_Member[HashId] = GraphicsInterface::Create##Resource(specs);\
	return m_Member[HashId];\
}\
void Nexus::ResourcePool::Deallocate##Resource(UUID HashId)\
{\
	if(m_Member.contains(HashId))\
		m_Member.erase(HashId);\
}\

#define RESOURCE_ALLOC_METHOD_REF_IMP(Resource,Specs,m_Member)\
Nexus::Ref<Nexus::##Resource> Nexus::ResourcePool::Allocate##Resource(const Specs& specs,UUID HashId)\
{\
	if(!m_Member.contains(HashId))\
		m_Member[HashId] = CreateRef<Resource>(specs);\
	return m_Member[HashId];\
}\
void Nexus::ResourcePool::Deallocate##Resource(UUID HashId)\
{\
	if(m_Member.contains(HashId))\
		m_Member.erase(HashId);\
}\

RESOURCE_ALLOC_METHOD_GI_IMP(Texture,TextureSpecification,m_Textures)
RESOURCE_ALLOC_METHOD_REF_IMP(RenderableMesh,MeshSpecification,m_RenderableMeshes)

Nexus::Ref<Nexus::MaterialTable> Nexus::ResourcePool::AllocateMaterialTable(const MaterialTableSpecification& specs, UUID HashID)
{
	if (m_MaterialTables.contains(HashID))
		return m_MaterialTables[HashID];

	std::vector<RenderableMaterial> materials;

	std::vector<UUID> textures;
	for (auto& tex : specs.textures)
	{
		UUID Id; AllocateTexture(tex, Id);
		textures.push_back(Id);
	}

	//std::vector<UUID> samplers;
	//for (auto& samp : specs.samplers)
	//{
	//	UUID Id;
	//	AllocateSampler(samp, Id);
	//	samplers.push_back(Id);
	//}

	for (auto& mat : specs.materials)
	{
		auto& m = materials.emplace_back();
		m.m_Params = mat.params;
		m.m_AlbedoMap = m_Textures[ textures[mat.albedoMap] ];
		m.m_NormalMap = m_Textures[ textures[mat.NormalMap] ];
		m.m_MetalicRoughnessMap = m_Textures[ textures[mat.MetalicRoughnesMap] ];
	}

	m_MaterialTables[HashID] = CreateRef<MaterialTable>(materials);
	return m_MaterialTables[HashID];
}

void Nexus::ResourcePool::DeallocateMaterialTable(UUID HashId)
{
	if (m_MaterialTables.contains(HashId))
		m_MaterialTables.erase(HashId);
}
