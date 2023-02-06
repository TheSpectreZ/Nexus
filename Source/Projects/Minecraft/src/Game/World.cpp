#include "World.h"
#include <vector>

#include "DebugUtils/Logger.h"

std::tuple<Vertex, Vertex, Vertex, Vertex> GetVertex(Voxel::Face face, glm::vec3 color, glm::vec3 pos)
{
	Vertex v1, v2, v3, v4;

	switch (face)
	{
	case Voxel::FACE_Top:
		v1 = { { 0.5f, 0.5f,-0.5f} , color };
		v2 = { { 0.5f, 0.5f, 0.5f} , color };
		v3 = { {-0.5f, 0.5f, 0.5f} , color };
		v4 = { {-0.5f, 0.5f,-0.5f} , color };
		break;
	case Voxel::FACE_Down:
		v1 = { { 0.5f,-0.5f, 0.5f} , color };
		v2 = { { 0.5f,-0.5f,-0.5f} , color };
		v3 = { {-0.5f,-0.5f,-0.5f} , color };
		v4 = { {-0.5f,-0.5f, 0.5f} , color };
		break;
	case Voxel::FACE_Right:
		v1 = { { 0.5f, 0.5f,-0.5f} , color };
		v2 = { { 0.5f,-0.5f,-0.5f} , color };
		v3 = { { 0.5f,-0.5f, 0.5f} , color };
		v4 = { { 0.5f, 0.5f, 0.5f} , color };
		break;
	case Voxel::FACE_Left:
		v1 = { {-0.5f, 0.5f, 0.5f} , color };
		v2 = { {-0.5f,-0.5f, 0.5f} , color };
		v3 = { {-0.5f,-0.5f,-0.5f} , color };
		v4 = { {-0.5f, 0.5f,-0.5f} , color };
		break;
	case Voxel::FACE_Front:
		v1 = { { 0.5f, 0.5f, 0.5f} , color };
		v2 = { { 0.5f,-0.5f, 0.5f} , color };
		v3 = { {-0.5f,-0.5f, 0.5f} , color };
		v4 = { {-0.5f, 0.5f, 0.5f} , color };
		break;
	case Voxel::FACE_Back:
		v1 = { {-0.5f, 0.5f,-0.5f} , color };
		v2 = { {-0.5f,-0.5f,-0.5f} , color };
		v3 = { { 0.5f,-0.5f,-0.5f} , color };
		v4 = { { 0.5f, 0.5f,-0.5f} , color };
		break;
	default:
		break;
	}

	v1.position += pos;
	v2.position += pos;
	v3.position += pos;
	v4.position += pos;

	return { v1,v2,v3,v4 };
}

void Chunk::Create()
{
	// Voxel
	{
		for (uint32_t y = 0; y < chunkSize; y++)
		{
			for (uint32_t z = 0; z < chunkSize; z++)
			{
				for (uint32_t x = 0; x < chunkSize; x++)
				{
					m_voxels[y][z][x].color = glm::normalize(glm::vec3(x, y, z));

					if (y == 0)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Down] = true;
					}
					else if (y == chunkSize - 1)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Top] = true;
					}

					if (x == 0)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Left] = true;
					}
					else if (x == chunkSize - 1)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Right] = true;
					}

					if (z == 0)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Back] = true;
					}
					else if (z == chunkSize - 1)
					{
						m_voxels[y][z][x].faces[(uint32_t)Voxel::FACE_Front] = true;
					}

				}
			}
		}
	}

	uint32_t indexCount = 0;

	// Vertex
	{
		std::vector<Vertex> vertices;

		for (uint32_t y = 0; y < chunkSize; y++)
		{
			for (uint32_t z = 0; z < chunkSize; z++)
			{
				for (uint32_t x = 0; x < chunkSize; x++)
				{
					for(uint32_t face = 0;face < 6;face++)
					{
						if (m_voxels[y][z][x].faces[face])
						{
							auto [v1, v2, v3, v4] = GetVertex((Voxel::Face)face, m_voxels[y][z][x].color,glm::vec3(x,y,z));
							
							vertices.push_back(v1);
							vertices.push_back(v2);
							vertices.push_back(v3);
							vertices.push_back(v4);
						}
					}
				}
			}
		}
		
		m_vb.Create(vertices.size(), sizeof(Vertex), vertices.data());

		indexCount = (vertices.size() / 4) * 6;
		vertices.clear();
	}

	// Index
	{
		uint32_t* indices = new uint32_t[indexCount];

		uint32_t k = 0;
		for (uint32_t i = 0; i < indexCount; i += 6)
		{
			indices[i] = k + 0;
			indices[i + 1] = k + 1;
			indices[i + 2] = k + 2;
			indices[i + 3] = k + 2;
			indices[i + 4] = k + 3;
			indices[i + 5] = k + 0;

			k += 4;
		}

		m_ib.Create(indexCount, sizeof(uint32_t), indices);

		delete[] indices;
	}
}

void Chunk::Destroy()
{
	m_vb.Destroy();
	m_ib.Destroy();
}

void Chunk::Render(VkCommandBuffer cmd)
{
	m_ib.Bind(cmd);
	m_vb.Bind(cmd);

	vkCmdDrawIndexed(cmd, m_ib.GetIndexCount(), 1, 0, 0, 0);
}

void World::Create()
{
	chunk.Create();
}

void World::Destroy()
{
	chunk.Destroy();
}

void World::Render(VkCommandBuffer cmd)
{
	chunk.Render(cmd);
}
