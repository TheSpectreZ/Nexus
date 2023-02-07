#include "World.h"
#include <vector>
#include "Graphics/Presenter.h"
#include "DebugUtils/Logger.h"

#include "Noise.h"
#include <iostream>

std::tuple<Vertex, Vertex, Vertex, Vertex> GetVertex(Voxel::Face face, glm::vec3 color, glm::vec3 pos)
{
	Vertex v1, v2, v3, v4;

	switch (face)
	{
	case Voxel::FACE_Top:
		v1 = { { 0.5f, 0.5f,-0.5f} , {color,0.9f} };
		v2 = { { 0.5f, 0.5f, 0.5f} , {color,0.9f} };
		v3 = { {-0.5f, 0.5f, 0.5f} , {color,0.9f} };
		v4 = { {-0.5f, 0.5f,-0.5f} , {color,0.9f} };
		break;
	case Voxel::FACE_Down:
		v1 = { { 0.5f,-0.5f, 0.5f} , {color,0.4f} };
		v2 = { { 0.5f,-0.5f,-0.5f} , {color,0.4f} };
		v3 = { {-0.5f,-0.5f,-0.5f} , {color,0.4f} };
		v4 = { {-0.5f,-0.5f, 0.5f} , {color,0.4f} };
		break;
	case Voxel::FACE_Right:
		v1 = { { 0.5f, 0.5f,-0.5f} , {color,0.7f} };
		v2 = { { 0.5f,-0.5f,-0.5f} , {color,0.7f} };
		v3 = { { 0.5f,-0.5f, 0.5f} , {color,0.7f} };
		v4 = { { 0.5f, 0.5f, 0.5f} , {color,0.7f} };
		break;
	case Voxel::FACE_Left:
		v1 = { {-0.5f, 0.5f, 0.5f} , {color,0.3f} };
		v2 = { {-0.5f,-0.5f, 0.5f} , {color,0.3f} };
		v3 = { {-0.5f,-0.5f,-0.5f} , {color,0.3f} };
		v4 = { {-0.5f, 0.5f,-0.5f} , {color,0.3f} };
		break;
	case Voxel::FACE_Front:
		v1 = { { 0.5f, 0.5f, 0.5f} , {color,0.8f} };
		v2 = { { 0.5f,-0.5f, 0.5f} , {color,0.8f} };
		v3 = { {-0.5f,-0.5f, 0.5f} , {color,0.8f} };
		v4 = { {-0.5f, 0.5f, 0.5f} , {color,0.8f} };
		break;
	case Voxel::FACE_Back:
		v1 = { {-0.5f, 0.5f,-0.5f} , {color,0.4f} };
		v2 = { {-0.5f,-0.5f,-0.5f} , {color,0.4f} };
		v3 = { { 0.5f,-0.5f,-0.5f} , {color,0.4f} };
		v4 = { { 0.5f, 0.5f,-0.5f} , {color,0.4f} };
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

float Chunk::threshold = 0.f;

void Chunk::Create()
{
	GenerateVoxels();
	GenerateMesh();
}

void Chunk::Destroy()
{
	m_vb.Destroy();
	m_ib.Destroy();
}

void Chunk::Update()
{
	Destroy();
	Create();
}

void Chunk::Render(VkCommandBuffer cmd)
{
	m_vb.Bind(cmd);
	m_ib.Bind(cmd);

	vkCmdDrawIndexed(cmd, m_ib.GetIndexCount(), 1, 0, 0, 0);
}

void Chunk::GenerateVoxels()
{
	srand(time(0));

	const siv::PerlinNoise::seed_type seed = rand();
	const siv::PerlinNoise perlin{ seed };

	double maxN = std::numeric_limits<double>::min(), 
		minN = std::numeric_limits<double>::max();

	for (uint32_t y = 0; y < chunkSize; y++)
	{
		for (uint32_t z = 0; z < chunkSize; z++)
		{
			for (uint32_t x = 0; x < chunkSize; x++)
			{
				double noise = perlin.noise3D(x / scale, y / scale, z / scale);

				if (noise > maxN)
					maxN = noise;

				if (noise < minN)
					minN = noise;

				m_voxels[y][z][x].color = glm::normalize(glm::vec3(x, y, z) * glm::vec3(rand()));

				if (noise > threshold)
					m_voxels[y][z][x].IsAir = true;
				else
					m_voxels[y][z][x].IsAir = false;
			}
		}
	}

}

void Chunk::GenerateMesh()
{
	std::vector<Vertex> vertices;

	std::vector<std::tuple<Vertex, Vertex, Vertex, Vertex>> verts;

	for (uint32_t y = 0; y < chunkSize; y++)
	{
		for (uint32_t z = 0; z < chunkSize; z++)
		{
			for (uint32_t x = 0; x < chunkSize; x++)
			{
				verts.clear();

				if (!m_voxels[y][z][x].IsAir)
				{
					if (x == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Left, m_voxels[y][z][x].color, { x,y,z }));
					else if (x == chunkSize - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Right, m_voxels[y][z][x].color, { x,y,z }));

					if (y == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Down, m_voxels[y][z][x].color, { x,y,z }));
					else if (y == chunkSize - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Top, m_voxels[y][z][x].color, { x,y,z }));

					if (z == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Back, m_voxels[y][z][x].color, { x,y,z }));
					else if (z == chunkSize - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Front, m_voxels[y][z][x].color, { x,y,z }));
				}
				else
				{
					if (y > 0 && !m_voxels[y - 1][z][x].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Top, m_voxels[y - 1][z][x].color, { x,y - 1,z }));

					if (y < chunkSize - 1 && !m_voxels[y + 1][z][x].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Down, m_voxels[y + 1][z][x].color, { x,y + 1,z }));

					if (x > 0 && !m_voxels[y][z][x - 1].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Right, m_voxels[y][z][x - 1].color, { x - 1,y,z }));

					if (x < chunkSize - 1 && !m_voxels[y][z][x + 1].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Left, m_voxels[y][z][x + 1].color, { x + 1,y,z }));

					if (z > 0 && !m_voxels[y][z - 1][x].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Front, m_voxels[y][z - 1][x].color, { x,y,z - 1 }));

					if (z < chunkSize - 1 && !m_voxels[y][z + 1][x].IsAir)
						verts.emplace_back(GetVertex(Voxel::FACE_Back, m_voxels[y][z + 1][x].color, { x,y,z + 1 }));
				}

				for (auto& v : verts)
				{
					auto& [a, b, c, d] = v;

					vertices.emplace_back(a);
					vertices.emplace_back(b);
					vertices.emplace_back(c);
					vertices.emplace_back(d);
				}

			}
		}
	}
	
	m_vb.Create(vertices.size(),sizeof(Vertex),vertices.data());
	
	uint32_t indexCount = ((uint32_t)vertices.size() / 4) * 6;
	vertices.clear();

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

void World::Update()
{
	Nexus::Graphics::Presenter::WaitForDevice();

	chunk.Update();
}
