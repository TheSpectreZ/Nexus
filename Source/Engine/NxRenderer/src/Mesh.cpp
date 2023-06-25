#include "NxRenderer/Mesh.h"

Nexus::RenderableMesh::RenderableMesh(const RenderableMeshSpecification& specs)
{
	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	bSpecs.type = BufferType::Vertex;
	bSpecs.size = specs.MeshVerticesSize;
	bSpecs.data = specs.MeshVerticesData;

	m_VertexBuffer = GraphicsInterface::CreateBuffer(bSpecs);

	bSpecs.type = BufferType::Index;
	bSpecs.size = specs.MeshIndicesSize;
	bSpecs.data = specs.MeshIndicesData;

	m_IndexBuffer = GraphicsInterface::CreateBuffer(bSpecs);
}

Nexus::RenderableMesh::~RenderableMesh()
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}
