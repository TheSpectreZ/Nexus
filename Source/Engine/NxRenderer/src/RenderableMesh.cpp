#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

Nexus::RenderableMesh::RenderableMesh(const Meshing::Mesh& mesh)
{
	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	bSpecs.type = BufferType::Vertex;
	bSpecs.size = (uint32_t)mesh.vertices.size() * sizeof(Meshing::Vertex);
	bSpecs.data = (void*)mesh.vertices.data();

	m_VB = GraphicsInterface::CreateBuffer(bSpecs);

	bSpecs.type = BufferType::Index;
	bSpecs.size = (uint32_t)mesh.indices.size() * sizeof(uint32_t);
	bSpecs.data = (void*)mesh.indices.data();

	m_IB = GraphicsInterface::CreateBuffer(bSpecs);

	m_Submeshes = mesh.submeshes;
}

Nexus::RenderableMesh::~RenderableMesh()
{
	m_VB.reset();
	m_IB.reset();
	m_Submeshes.clear();
}
