#include "NxGraphics/Renderables.h"

Nexus::Mesh::Mesh(const MeshSpecifications& specs)
{
	m_Elements = specs.elements;
	m_MaterialIndices = specs.materialIndices;
}