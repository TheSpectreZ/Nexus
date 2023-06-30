#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

Nexus::RenderableMesh::RenderableMesh(const RenderableMeshSpecification& specs)
{
	auto& mSpecs = specs.meshSpecs;

	std::vector<MeshVertex> verts;
	std::vector<uint32_t> indexs;

	size_t vSize = 0, iSize = 0;
	for(auto& element : mSpecs.elements)
	{
		vSize += element.Vertices.size();
		iSize += element.Indices.size();
	}

	verts.reserve(vSize);
	indexs.reserve(iSize);
	
	for (auto& element : mSpecs.elements)
	{
		verts.insert(verts.end(), element.Vertices.begin(), element.Vertices.end());
		indexs.insert(indexs.end(), element.Indices.begin(), element.Indices.end());
	}

	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	bSpecs.type = BufferType::Vertex;
	bSpecs.size = (uint32_t)verts.size() * sizeof(MeshVertex);
	bSpecs.data = verts.data();

	m_VertexBuffer = GraphicsInterface::CreateBuffer(bSpecs);

	bSpecs.type = BufferType::Index;
	bSpecs.size = (uint32_t)indexs.size() * sizeof(uint32_t);
	bSpecs.data = indexs.data();

	m_IndexBuffer = GraphicsInterface::CreateBuffer(bSpecs);
}

Nexus::RenderableMesh::~RenderableMesh()
{
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();
}
