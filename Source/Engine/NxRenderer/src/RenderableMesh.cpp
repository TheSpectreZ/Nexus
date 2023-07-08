#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

Nexus::RenderableMesh::RenderableMesh(const MeshSpecification& specs)
{
	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	bSpecs.type = BufferType::Vertex;
	bSpecs.size = (uint32_t)specs.mesh.Vertices.size() * sizeof(MeshVertex);
	bSpecs.data = (void*)specs.mesh.Vertices.data();

	m_VertexBuffer = GraphicsInterface::CreateBuffer(bSpecs);

	bSpecs.type = BufferType::Index;
	bSpecs.size = (uint32_t)specs.mesh.Indices.size() * sizeof(uint32_t);
	bSpecs.data = (void*)specs.mesh.Indices.data();

	m_IndexBuffer = GraphicsInterface::CreateBuffer(bSpecs);

	m_Submeshs = specs.submeshes;
}

Nexus::RenderableMesh::~RenderableMesh()
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}
