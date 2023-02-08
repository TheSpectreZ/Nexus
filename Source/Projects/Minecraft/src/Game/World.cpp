#include "World.h"
#include <vector>
#include "Graphics/Presenter.h"
#include "DebugUtils/Logger.h"
#include "Platform/Input.h"

#include "Noise.h"

#include <iostream>
#include <execution>

std::vector<int> World::Iters = { -7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7 };

glm::vec2 GetBlockIndex(Block Type, Voxel::Face face)
{
	glm::vec2 coord{};

	switch (Type)
	{
	case Block::GRASS:
		switch (face)
		{
		case Voxel::FACE_Top:
			coord = { 0,0 };
			break;
		case Voxel::FACE_Down:
			coord = { 2,0 };
			break;
		default:
			coord = { 3,0 };
			break;
		}
		break;
	case Block::STONE:
		coord = { 1,0 };
		break;
	case Block::BEDROCK:
		coord = { 1,1 };
		break;
	case Block::DIRT:
		coord = { 2,0 };
		break;
	case Block::SAND:
		coord = { 2,1 };
		break;
	case Block::SNOW:
		coord = { 2,4 };
		break;
	case Block::IRON_ORE:
		coord = { 1,2 };
		break;
	case Block::OAK_TRUNK:
		switch (face)
		{
		case Voxel::FACE_Top:
			coord = { 5,1 };
			break;
		case Voxel::FACE_Down:
			coord = { 5,1 };
			break;
		default:
			coord = { 4,1 };
			break;
		}
		break;
	case Block::OAK_LEAF:
		coord = { 1,8 };
		break;
	default:
		break;
	}

	return coord;
}

std::tuple<Vertex, Vertex, Vertex, Vertex> GetVertex(Voxel::Face face, Block Type, glm::vec3 pos)
{
	Vertex v1, v2, v3, v4;

	static glm::vec2 ImageSize = { 256,256 };
	static glm::vec2 spriteSize = { 16,16 };
	glm::vec2 Index = GetBlockIndex(Type, face);
	
	std::array<glm::vec2, 4> coords =
	{
		glm::vec2((Index.x + 1) * spriteSize.x / ImageSize.x,  Index.y		* spriteSize.y / ImageSize.y),
		glm::vec2((Index.x + 1) * spriteSize.x / ImageSize.x, (Index.y + 1) * spriteSize.y / ImageSize.y),
		glm::vec2( Index.x		* spriteSize.x / ImageSize.x, (Index.y + 1) * spriteSize.y / ImageSize.y),
		glm::vec2( Index.x		* spriteSize.x / ImageSize.x,  Index.y		* spriteSize.y / ImageSize.y),
	};

	switch (face)
	{
	case Voxel::FACE_Top:	
		v1 = { { 0.5f, 0.5f,-0.5f} , {coords[0],0.9f}  };
		v2 = { { 0.5f, 0.5f, 0.5f} , {coords[1],0.9f}  };
		v3 = { {-0.5f, 0.5f, 0.5f} , {coords[2],0.9f}  };
		v4 = { {-0.5f, 0.5f,-0.5f} , {coords[3],0.9f}  };
		break;
	case Voxel::FACE_Down:
		v1 = { { 0.5f,-0.5f, 0.5f} , {coords[0],0.4f} };
		v2 = { { 0.5f,-0.5f,-0.5f} , {coords[1],0.4f} };
		v3 = { {-0.5f,-0.5f,-0.5f} , {coords[2],0.4f} };
		v4 = { {-0.5f,-0.5f, 0.5f} , {coords[3],0.4f} };
		break;
	case Voxel::FACE_Right:
		v1 = { { 0.5f, 0.5f,-0.5f} , {coords[0],0.7f} };
		v2 = { { 0.5f,-0.5f,-0.5f} , {coords[1],0.7f} };
		v3 = { { 0.5f,-0.5f, 0.5f} , {coords[2],0.7f} };
		v4 = { { 0.5f, 0.5f, 0.5f} , {coords[3],0.7f} };
		break;
	case Voxel::FACE_Left:
		v1 = { {-0.5f, 0.5f, 0.5f} , {coords[0],0.3f}  };
		v2 = { {-0.5f,-0.5f, 0.5f} , {coords[1],0.3f}  };
		v3 = { {-0.5f,-0.5f,-0.5f} , {coords[2],0.3f}  };
		v4 = { {-0.5f, 0.5f,-0.5f} , {coords[3],0.3f}  };
		break;
	case Voxel::FACE_Front:
		v1 = { { 0.5f, 0.5f, 0.5f} , {coords[0],0.8f} };
		v2 = { { 0.5f,-0.5f, 0.5f} , {coords[1],0.8f} };
		v3 = { {-0.5f,-0.5f, 0.5f} , {coords[2],0.8f} };
		v4 = { {-0.5f, 0.5f, 0.5f} , {coords[3],0.8f} };
		break;
	case Voxel::FACE_Back:
		v1 = { {-0.5f, 0.5f,-0.5f} , {coords[0],0.4f} };
		v2 = { {-0.5f,-0.5f,-0.5f} , {coords[1],0.4f} };
		v3 = { { 0.5f,-0.5f,-0.5f} , {coords[2],0.4f} };
		v4 = { { 0.5f, 0.5f,-0.5f} , {coords[3],0.4f} };
		break;
	default:
		break;
	}

	float halfDist = Chunk::chunkSize / 2.f;

	v1.position = v1.position + pos - glm::vec3(halfDist, 0.f, halfDist);
	v2.position = v2.position + pos - glm::vec3(halfDist, 0.f, halfDist);
	v3.position = v3.position + pos - glm::vec3(halfDist, 0.f, halfDist);
	v4.position = v4.position + pos - glm::vec3(halfDist, 0.f, halfDist);

	return { v1,v2,v3,v4 };
}

void Chunk::Create(glm::vec2 coordinate,uint32_t seed)
{
	m_coordinate = coordinate;

	m_voxels = new Voxel**[chunkheight];

	for (uint32_t i = 0; i < chunkheight; i++)
	{
		m_voxels[i] = new Voxel * [chunkSize];
		for (uint32_t j = 0; j < chunkSize; j++)
		{
			m_voxels[i][j] = new Voxel[chunkSize];
		}
	}

	GenerateVoxels(seed);
	GenerateMesh();

	for (uint32_t i = 0; i < chunkheight; i++)
	{
		for (uint32_t j = 0; j < chunkSize; j++)
		{
			delete[] m_voxels[i][j];
		}
		delete[] m_voxels[i];
	}
	delete[] m_voxels;

	Created = true;
}

void Chunk::Destroy()
{
	m_vb.Destroy();
	m_ib.Destroy();
}

void Chunk::Render(VkCommandBuffer cmd)
{
	m_vb.Bind(cmd);
	m_ib.Bind(cmd);

	vkCmdDrawIndexed(cmd, m_ib.GetIndexCount(), 1, 0, 0, 0);
}

void Chunk::GenerateVoxels(uint32_t seed)
{
	const siv::PerlinNoise perlin{ seed };

	uint32_t** heightMap = new uint32_t * [chunkSize];

	for (uint32_t i = 0; i < chunkSize; i++)
		heightMap[i] = new uint32_t[chunkSize];

	float halfDist = Chunk::chunkSize / 2.f;
	float NoiseScale = 14.f;

	for (uint32_t z = 0; z < chunkSize; z++)
	{
		for (uint32_t x = 0; x < chunkSize; x++)
		{
			heightMap[z][x] = chunkheight * 0.5 + (uint32_t)(perlin.normalizedOctave2D_01((x + m_coordinate.x + 0.01f) / NoiseScale, (z + m_coordinate.y + 0.01f) / NoiseScale, 4,0.2f) * (chunkheight * 0.5));
		}
	}

	std::vector<glm::vec3> TreeGenerationBlocks;

	for (uint32_t y = 0; y < chunkheight; y++)
	{
		for (uint32_t z = 0; z < chunkSize; z++)
		{
			for (uint32_t x = 0; x < chunkSize; x++)
			{
				m_voxels[y][z][x].color = glm::normalize(glm::vec3(x, y, z));

				if (y == 0)
					m_voxels[y][z][x].Type = Block::BEDROCK;
				else if (y > heightMap[z][x])
					m_voxels[y][z][x].Type = Block::AIR;
				else if (y == heightMap[z][x])
				{
					if (y > chunkheight * 0.65)
					{
						m_voxels[y][z][x].Type = Block::GRASS;

						uint32_t treeGen = rand() % 1000;
						if (treeGen < 10)
							TreeGenerationBlocks.push_back({ x,y,z });
					}
					else
						m_voxels[y][z][x].Type = Block::SAND;
				}
				else if (y > heightMap[z][x] - 4 && y < heightMap[z][x])
					m_voxels[y][z][x].Type = Block::DIRT;
				else 
				{
					double noise = perlin.normalizedOctave3D_01((x + m_coordinate.x + 0.01f) / NoiseScale, (y + 0.01f)/NoiseScale, (z + m_coordinate.y + 0.01f) / NoiseScale, 1);
					if (noise > 0.6)
						m_voxels[y][z][x].Type = Block::STONE;
					else
						m_voxels[y][z][x].Type = Block::AIR;
				}
			}
		}
	}

	for (auto& t : TreeGenerationBlocks)
	{		
		GenerateTree(t, heightMap);
	}

	for (uint32_t i = 0; i < chunkSize; i++)
		delete[] heightMap[i];

	delete[] heightMap;
}

void Chunk::GenerateMesh()
{
	std::vector<Vertex> vertices;

	std::vector<std::tuple<Vertex, Vertex, Vertex, Vertex>> verts;

	for (uint32_t y = 0; y < chunkheight; y++)
	{
		for (uint32_t z = 0; z < chunkSize; z++)
		{
			for (uint32_t x = 0; x < chunkSize; x++)
			{
				verts.clear();

				if (m_voxels[y][z][x].Type != Block::AIR)
				{
					glm::vec3 c = glm::vec3(x + m_coordinate.x, y, z + m_coordinate.y);

					if (x == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Left,m_voxels[y][z][x].Type, c));
					else if (x == chunkSize - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Right, m_voxels[y][z][x].Type, c));

					if (y == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Down, m_voxels[y][z][x].Type, c));
					else if (y == chunkheight - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Top, m_voxels[y][z][x].Type, c));

					if (z == 0)
						verts.emplace_back(GetVertex(Voxel::FACE_Back, m_voxels[y][z][x].Type, c));
					else if (z == chunkSize - 1)
						verts.emplace_back(GetVertex(Voxel::FACE_Front, m_voxels[y][z][x].Type, c));
				}
				else
				{
					if (y > 0 && m_voxels[y - 1][z][x].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Top, m_voxels[y-1][z][x].Type, glm::vec3(x + m_coordinate.x, y-1, z + m_coordinate.y)));

					if (y < chunkheight - 1 && m_voxels[y + 1][z][x].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Down, m_voxels[y+1][z][x].Type, glm::vec3(x + m_coordinate.x, y+1, z + m_coordinate.y) ));

					if (x > 0 && m_voxels[y][z][x - 1].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Right, m_voxels[y][z][x-1].Type, glm::vec3(x-1 + m_coordinate.x, y, z + m_coordinate.y) ));

					if (x < chunkSize - 1 && m_voxels[y][z][x + 1].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Left, m_voxels[y][z][x+1].Type, glm::vec3(x+1 + m_coordinate.x, y, z + m_coordinate.y) ));

					if (z > 0 && m_voxels[y][z - 1][x].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Front, m_voxels[y][z-1][x].Type, glm::vec3(x + m_coordinate.x, y, z-1 + m_coordinate.y) ));

					if (z < chunkSize - 1 && m_voxels[y][z + 1][x].Type != Block::AIR)
						verts.emplace_back(GetVertex(Voxel::FACE_Back, m_voxels[y][z+1][x].Type, glm::vec3(x + m_coordinate.x, y, z+1 + m_coordinate.y) ));
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

void Chunk::GenerateTree(glm::vec3 basePos , uint32_t** HeightMap)
{
	uint32_t z = basePos.z, y = basePos.y, x = basePos.x;

	if (y >= chunkheight - 9)
		return;

	uint32_t height = 3 + rand() % 3;

	for (uint32_t i = 1; i <= height; i++)
	{
		m_voxels[y + i][z][x].Type = Block::OAK_TRUNK;
	}
	
	uint32_t hOff = 0;
	for (int32_t k = -3; k < 0; k++)
	{
		hOff++;

		for (int32_t i = k; i <= -k; i++)
		{
			for (int32_t j = k; j <= -k; j++)
			{
				if ((z + i) >= 0 && (z + i) < chunkSize && (x + j) >= 0 && (x + j) < chunkSize && (y + height + hOff) >= 0 && (y + height + hOff) < chunkheight)
				{
					if (i == 0 && j == 0 && k < -1)
						m_voxels[y + height + hOff][z][x].Type = Block::OAK_TRUNK;

					m_voxels[y + height + hOff][z + i][x + j].Type = Block::OAK_LEAF;
				}
			}
		}
	}
}

void World::Create()
{	
	srand(time(0));

	m_worldSeed = rand();

	m_Player.Create();
	UpdateActiveChunks();
}

void World::Destroy()
{
	for (auto& chunk : m_chunks)
		chunk.second.Destroy();
}

void World::Render(VkCommandBuffer cmd)
{	
	for (auto& chunk : m_ActiveChunks)
		chunk->Render(cmd);
}

bool regenerate = true;

void World::Update()
{
	m_Player.Update();

	auto playerChunk = glm::ivec2(m_Player.GetPosition().x, m_Player.GetPosition().z) / ChunkSize;

	if (playerChunk != m_CurrentChunk)
	{
		NEXUS_LOG_WARN("{0},{1} : {2},{3}", playerChunk.x, playerChunk.y, m_CurrentChunk.x, m_CurrentChunk.y);

		m_CurrentChunk = playerChunk;
		UpdateActiveChunks();
	}
}

void World::UpdateActiveChunks()
{
	m_ActiveChunks.clear();

	std::for_each(Iters.begin(), Iters.end(), [this](int i)
		{
			std::for_each(Iters.begin(), Iters.end(), [this, i](int j)
				{
					glm::vec2 coord = { m_CurrentChunk.x - i, m_CurrentChunk.y - j };

					if (!m_chunks.contains(coord))
					{
						m_chunks[coord] = Chunk();
						m_chunks[coord].Create(coord * glm::vec2(ChunkSize),m_worldSeed);
					}

					m_ActiveChunks.push_back(&m_chunks[coord]);
				});
		});
}
