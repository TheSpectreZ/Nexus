#include "nxpch.h"
#include "Vertex.h"

std::vector<Nexus::VertexBindInfo> Nexus::StaticMeshVertex::GetBindings()
{
	std::vector<Nexus::VertexBindInfo> Info(1);

	Info[0].binding = 0;
	Info[0].stride = sizeof(StaticMeshVertex);
	Info[0].inputRate = VertexBindInfo::INPUT_RATE_VERTEX;

	return Info;
}

std::vector<Nexus::VertexAttribInfo> Nexus::StaticMeshVertex::GetAttributes()
{
	std::vector<Nexus::VertexAttribInfo> Info(5);

	Info[0].binding = 0;
	Info[0].location = 0;
	Info[0].offset = 0;
	Info[0].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;

	Info[1].binding = 0;
	Info[1].location = 1;
	Info[1].offset = sizeof(glm::vec3);
	Info[1].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;
	
	Info[2].binding = 0;
	Info[2].location = 2;
	Info[2].offset = sizeof(glm::vec3) * 2;
	Info[2].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;
	
	Info[3].binding = 0;
	Info[3].location = 3;
	Info[3].offset = sizeof(glm::vec3) * 3;
	Info[3].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;
	
	Info[4].binding = 0;
	Info[4].location = 4;
	Info[4].offset = sizeof(glm::vec3) * 4;
	Info[4].format = VertexAttribInfo::ATTRIB_FORMAT_VEC2;

	return Info;
}