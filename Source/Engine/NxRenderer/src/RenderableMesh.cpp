#include "NxRenderer/RenderableMesh.h"
#include "NxRenderer/GraphicsInterface.h"

Nexus::RenderableMesh::RenderableMesh(const std::vector<Meshing::Mesh>& meshes)
{
	BufferSpecification bSpecs{};
	bSpecs.cpuMemory = false;

	for (auto& mesh : meshes)
	{
		auto& m = m_Meshes.emplace_back();

		bSpecs.type = BufferType::Vertex;
		bSpecs.size = (uint32_t)mesh.vertices.size() * sizeof(Meshing::Vertex);
		bSpecs.data = (void*)mesh.vertices.data();

		m.vb = GraphicsInterface::CreateBuffer(bSpecs);

		bSpecs.type = BufferType::Index;
		bSpecs.size = (uint32_t)mesh.indices.size() * sizeof(uint32_t);
		bSpecs.data = (void*)mesh.indices.data();

		m.ib = GraphicsInterface::CreateBuffer(bSpecs);

		m.sb = mesh.submeshes;
	}

}

Nexus::RenderableMesh::~RenderableMesh()
{
	m_Meshes.clear();
}
